#include <Arduino.h>
#include "leds.h"
#include "filesystem.h"
#include "serialmanager.h"
#include "wifi-manager.h"
#include "mqttmanager.h"

void setup()
{
    Serial.begin(115200);
    delay(100);
    Serial.println(F("Initializing"));
    Serial.println(ESP.getFreeHeap());

    setupLeds();
    tweenToColor(255, 255, 255, 255);
    delay(2000);
    tweenToColor(255, 0, 0, 0);
    setupFilesystem();
    setupSerial();

    delay(2000);
    tweenToColor(255, 165, 0, 0);

    if (!setupWifi())
    {
        tweenToColor(255, 0, 255, 0);
        return;
    };
    delay(2000);
    tweenToColor(0, 0, 255, 0);
    setupMqtt();
    tweenToColor(0, 0, 0, 0);
}

void loop()
{
    serialLoop();
    if (WiFi.status() == WL_CONNECTED)
    {
        mqttloop();
        ledsloop();
    }
}