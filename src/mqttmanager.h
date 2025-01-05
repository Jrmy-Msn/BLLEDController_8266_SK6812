#ifndef _MQTTMANAGER
#define _MQTTMANAGER

#define ARDUINOJSON_ENABLE_STD_STREAM 1

#include <Arduino.h>
// #include <StreamUtils.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "AutoGrowBufferStream.h"
#include <Stream.h>

#include "types.h"
#include "config.h"
#include "leds.h"

#include <ESP8266WiFi.h>
static int mqttbuffer = 4096;
// static int mqttdocument = 4096;

WiFiClientSecure wifiSecureClient;
PubSubClient mqttClient(wifiSecureClient);

AutoGrowBufferStream stream;

String device_topic;
String report_topic;
String request_topic;
String clientId = "BLLED-";

unsigned long mqttattempt = (millis() - 3000);

String ParseHMSSeverity(int code)
{ // Provided by WolfWithSword
    int parsedcode(code >> 16);

    switch (parsedcode)
    {
    case 1:
        return F("Fatal");
    case 2:
        return F("Serious");
    case 3:
        return F("Common");
    case 4:
        return F("Info");
    default:;
    }
    return "";
}

JsonDocument getMqttPayloadFilter()
{
    JsonDocument filter;
    // filter["print"]["command"] = true;
    // filter["print"]["fail_reason"] = true;
    filter["print"]["gcode_state"] = true;
    // filter["print"]["print_gcode_action"] = true;
    // filter["print"]["print_real_action"] = true;
    filter["print"]["hms"] = true;
    // filter["print"]["home_flag"] = true;
    filter["print"]["lights_report"] = true;
    filter["print"]["stg_cur"] = true;
    // filter["print"]["print_error"] = true;
    // filter["print"]["wifi_signal"] = true;
    // filter["system"]["command"] = true;
    // filter["system"]["led_mode"] = true;
    // Make sure to add more here when needed
    return filter;
}

void connectMqtt()
{
    device_topic = String("device/") + globalVariables.serialNumber;
    report_topic = device_topic + String("/report");
    request_topic = device_topic + String("/request");

    if (!mqttClient.connected() && (millis() - mqttattempt) >= 3000)
    {
        if (mqttClient.connect(clientId.c_str(), "bblp", globalVariables.accessCode))
        {
            Serial.println(F("Connected to mqtt"));
            Serial.println(report_topic);
            mqttClient.subscribe(report_topic.c_str());
            printerVariables.online = true;
            updateleds();
        }
        else
        {
            switch (mqttClient.state())
            {
            case -4: // MQTT_CONNECTION_TIMEOUT
                Serial.println(F("MQTT TIMEOUT"));
                break;
            case -2: // MQTT_CONNECT_FAILED
                Serial.println(F("MQTT CONNECT_FAILED"));
                break;
            case -3: // MQTT_CONNECTION_LOST
                Serial.println(F("MQTT CONNECTION_LOST"));
                break;
            case -1: // MQTT_DISCONNECTED
                Serial.println(F("MQTT DISCONNECTEDT"));
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5: // MQTT UNAUTHORIZED
                Serial.println(F("MQTT UNAUTHORIZED"));
                ESP.restart();
                break;
            }
        }
    }
}

bool handle_message(JsonDocument message)
{
    bool Changed = false;

    if (message["print"]["stg_cur"].is<int>())
    {
        printerVariables.stage = message["print"]["stg_cur"];
        Changed = true;
    }
    else
    {
        Serial.println(F("stg_cur not in message"));
    }

    if (message["print"]["gcode_state"].is<String>())
    {
        String oldGcodeState = "";
        oldGcodeState = printerVariables.gcodeState;
        printerVariables.gcodeState = message["print"]["gcode_state"].as<String>();
        if (printerVariables.gcodeState == "FINISH")
        {
            if (printerVariables.finished == false && oldGcodeState != printerVariables.gcodeState)
            {
                printerVariables.finished = true;
                printerVariables.finishstartms = millis();
                Changed = true;
            }
        }
        else if (printerVariables.gcodeState == "FAILED")
        {
            printerVariables.finished = true;
            printerVariables.finishstartms = millis();
            Changed = true;
        }
        else if (oldGcodeState != printerVariables.gcodeState)
        {
            printerVariables.finished = false;
            Changed = true;
        }
    }

    if (message["print"]["lights_report"].is<JsonArray>())
    {
        JsonArray lightsReport = message["print"]["lights_report"];

        for (JsonObject light : lightsReport)
        {
            if (light["node"] == "chamber_light")
            {
                if (printerVariables.ledstate != (light["mode"] == "on"))
                {
                    printerVariables.ledstate = (light["mode"] == "on");
                    Changed = true;
                }
            }
        }
    }
    else
    {
        Serial.println(F("lights_report not in message"));
    }

    if (message["print"]["hms"].is<JsonArray>())
    {
        String oldHMSlevel = "";
        oldHMSlevel = printerVariables.parsedHMSlevel;
        printerVariables.hmsstate = false;
        printerVariables.parsedHMSlevel = "";

        if (message["print"]["hms"].as<JsonArray>().size() > 0)
        {
            for (const auto &hms : message["print"]["hms"].as<JsonArray>())
            {
                if (ParseHMSSeverity(hms["code"]) != "")
                {
                    printerVariables.hmsstate = true;
                    printerVariables.parsedHMSlevel = ParseHMSSeverity(hms["code"]);
                    printerVariables.parsedHMScode = ((uint64_t)hms["attr"] << 32) + (uint64_t)hms["code"];
                }
            }
            if (oldHMSlevel != printerVariables.parsedHMSlevel)
            {

                if (printerVariables.parsedHMScode == 0x0C0003000003000B)
                    printerVariables.overridestage = 10;
                if (printerVariables.parsedHMScode == 0x0300120000020001)
                    printerVariables.overridestage = 17;
                if (printerVariables.parsedHMScode == 0x0700200000030001)
                    printerVariables.overridestage = 6;
                if (printerVariables.parsedHMScode == 0x0300020000010001)
                    printerVariables.overridestage = 20;
                if (printerVariables.parsedHMScode == 0x0300010000010007)
                    printerVariables.overridestage = 21;

                Changed = true;
            }
        }
        else
        {
            Changed = true;
            printerVariables.overridestage = 999;
        }
    }

    return Changed;
}

void ParseCallback(char *topic, byte *payload, unsigned int length)
{
    JsonDocument filter = getMqttPayloadFilter();
    JsonDocument messageobject;
    stream.find("\"print\":[");
    do
    {
        DeserializationError deserializeError = deserializeJson(messageobject, payload, DeserializationOption::Filter(filter));
        if (messageobject.overflowed())
        {
            Serial.println(F("---------> JSON OVREFLOW"));
        }

        Serial.println(F("------------ MQTT MESSAGE ------------"));

        Serial.print(F("FreeHeap: "));
        Serial.println(ESP.getFreeHeap());
        Serial.print(F("DeserializeError : "));
        Serial.println(deserializeError.c_str());
        if (!deserializeError)
        {

            Serial.println(F("Mqtt payload:"));
            Serial.println();
            serializeJson(messageobject, Serial);
            Serial.println();

            if (handle_message(messageobject))
            {
                Serial.println(F("Updating from mqtt"));
                updateleds();
            }
        }
        else
        {
            Serial.println(F("Deserialize error while parsing mqtt"));
        }

        Serial.println(F("------------ END MQTT MESSAGE ------------"));
        Serial.println();
    } while (stream.findUntil(",", "]"));
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    stream.setTimeout(10000);
    ParseCallback(topic, (byte *)stream.get_buffer(), stream.current_length());
    stream.flush();
}

void setupMqtt()
{
    clientId += String(random(0xffff), HEX);

    strcpy(globalVariables.printerIP, printerConfig.printerIP);
    strcpy(globalVariables.accessCode, printerConfig.accessCode);
    strcpy(globalVariables.serialNumber, printerConfig.serialNumber);

    Serial.print(F("Setting up MQTT with ip : "));
    Serial.println(globalVariables.printerIP);
    wifiSecureClient.setInsecure();
    mqttClient.setBufferSize(mqttbuffer);
    mqttClient.setServer(globalVariables.printerIP, 8883);
    mqttClient.setStream(stream);
    mqttClient.setCallback(mqttCallback);
    Serial.println(F("Finished setting up MQTT, Attempting to connect"));
    connectMqtt();
}

void mqttloop()
{
    if (!mqttClient.connected())
    {
        printerVariables.online = false;
        updateleds();
        connectMqtt();
    }
    else
    {
        mqttClient.loop();
    }
}

#endif