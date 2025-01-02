#ifndef _LED
#define _LED

#include <Arduino.h>
#include "types.h"

#include <Adafruit_NeoPixel.h>
#define PIN D9
#define LED_COUNT 10
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRBW + NEO_KHZ800);

const int redPin = 5;
const int greenPin = 14;
const int bluePin = 4;
const int warmPin = 2;
const int coldPin = 0;

int currentRed = 0;
int currentGreen = 0;
int currentBlue = 0;
int currentWarm = 0;
int currentCold = 0;
int currentWhite = 0;

void colorSet(uint32_t c, uint8_t wait = 0)
{ // From NeoPixel Library
    for (uint16_t i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, c);
    }
    strip.show();
    delay(wait);
}

void snakeSet(uint32_t c, int length = 4, int duration = 100)
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        strip.clear();
        for (int j = 0; j < length; j++)
        {
            strip.setPixelColor(i - j, c);
        }
        strip.show();
        delay(duration);
    }
}

void snakeToColor(int targetRed, int targetGreen, int targetBlue, int targetWarm, int targetCold, int duration = 100)
{
    int targetWhite = round((targetCold + targetWarm) / 2);

    if (targetRed == currentRed && targetGreen == currentGreen && targetBlue == currentBlue && targetWarm == currentWarm && targetCold == currentCold)
    {
        return; // already at that color
    }

    currentRed = targetRed;
    currentGreen = targetGreen;
    currentBlue = targetBlue;
    currentWhite = targetWhite;

    uint32_t color = strip.Color(currentRed, currentGreen, currentBlue, currentWhite);
    snakeSet(color, 3);
}

void breathToColor(int targetRed, int targetGreen, int targetBlue, int targetWarm, int targetCold, int duration)
{
    int targetWhite = round((targetCold + targetWarm) / 2);

    if (targetRed == currentRed && targetGreen == currentGreen && targetBlue == currentBlue && targetWarm == currentWarm && targetCold == currentCold)
    {
        Serial.println(F("Leds already at that color"));
        return; // already at that color
    }

    currentRed = targetRed;
    currentGreen = targetGreen;
    currentBlue = targetBlue;
    currentWhite = targetWhite;
    uint32_t color = strip.Color(currentRed, currentGreen, currentBlue, currentWhite);

    strip.setBrightness(50);
    for (uint8_t b = 0; b < 50; b++)
    {
        strip.setBrightness(b * 255 / 255);
        for (uint16_t i = 0; i < strip.numPixels(); i++)
        {
            strip.setPixelColor(i, color);
        }
        strip.show();
        delay(30);
    }
    strip.setBrightness(50 * 255 / 255);
    for (uint16_t i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, color);
        strip.show();
        delay(50);
    }
    for (uint8_t b = 50; b > 0; b--)
    {
        strip.setBrightness(b * 255 / 255);
        for (uint16_t i = 0; i < strip.numPixels(); i++)
        {
            strip.setPixelColor(i, color);
        }
        strip.show();
        delay(50);
    }
}

void tweenToColor(int targetRed, int targetGreen, int targetBlue, int targetWarm, int targetCold, int duration)
{
    int targetWhite = round((targetCold + targetWarm) / 2);

    if (targetRed == currentRed && targetGreen == currentGreen && targetBlue == currentBlue && currentWhite == targetWhite)
    {
        Serial.println(F("Leds already at that color"));
        return; // already at that color
    }
    currentRed = targetRed;
    currentGreen = targetGreen;
    currentBlue = targetBlue;
    currentWhite = targetWhite;
    uint32_t color = strip.Color(currentRed, currentGreen, currentBlue, currentWhite);
    colorSet(color);
}

void updateleds()
{
    Serial.println(F("Updating leds"));

    if (printerVariables.hmsstate)
    {
    }

    // OFF

    if (printerVariables.online == false)
    {                                     // printer offline
        tweenToColor(0, 0, 0, 0, 0, 500); // OFF
        Serial.println(F("Printer offline, Turning Leds off"));
        return;
    }

    if (printerVariables.ledstate == false && !printerVariables.hmsstate)
    {                                     // replicate printer behaviour
        tweenToColor(0, 0, 0, 0, 0, 500); // OFF
        Serial.println(F("Chamber light off, Turning Leds off"));
        return;
    }

    if (printerVariables.stage == 9)
    {                                     // Scaning surface
        tweenToColor(0, 0, 0, 0, 0, 500); // LOW
        Serial.println(F("Scaning surface, Turning Leds low"));
        return;
    }

    // Inspecting First Layer
    if (printerVariables.stage == 10 || printerVariables.overridestage == 10)
    {                                     // Inspecting First Layer
        tweenToColor(0, 0, 0, 0, 0, 500); // LOW
        Serial.println(F("Inspecting First Layer, Turning Leds low"));
        return;
    }

    // Calibrating  MicroLidar
    if (printerVariables.stage == 12)
    {                                     // Calibrating  MicroLidar
        tweenToColor(0, 0, 0, 0, 0, 500); // LOW
        Serial.println(F("Calibrating  MicroLidar, Turning Leds low"));
        return;
    }

    if (printerVariables.stage == 6 || printerVariables.overridestage == 6)
    {                                       // Fillament runout
        tweenToColor(255, 0, 0, 0, 0, 500); // RED
        Serial.println(F("Fillament runout, Turning Leds red"));
        return;
    }

    if (printerVariables.stage == 17 || printerVariables.overridestage == 17)
    {                                       // Front Cover Removed
        tweenToColor(255, 0, 0, 0, 0, 500); // RED
        Serial.println(F("Front Cover Removed, Turning Leds red"));
        return;
    }

    if (printerVariables.stage == 20 || printerVariables.overridestage == 20)
    {                                       // Nozzle Temp fail
        tweenToColor(255, 0, 0, 0, 0, 500); // RED
        Serial.println(F("Nozzle Temp fail, Turning Leds red"));
        return;
    }

    if (printerVariables.stage == 21 || printerVariables.overridestage == 21)
    {                                       // Bed Temp Fail
        tweenToColor(255, 0, 0, 0, 0, 500); // RED
        Serial.println(F("Bed Temp fail, Turning Leds red"));
        return;
    }

    if ((millis() - printerVariables.finishstartms) <= 300000 && printerVariables.gcodeState == "FAILED")
    {                                       // BREATH
        tweenToColor(255, 0, 0, 0, 0, 500); // Red
        delay(300);
        tweenToColor(0, 0, 0, 0, 0, 500); // OFF
        delay(100);
        return;
    }

    // GREEN

    if ((millis() - printerVariables.finishstartms) <= 300000 && printerVariables.gcodeState == "FINISH")
    {
        tweenToColor(0, 255, 0, 0, 0, 500); // ON
        return;
    }

    // OTHER COLOR

    // Calibrating extrusion
    if (printerVariables.stage == 8)
    {                                         // Calibrating extrusion
        tweenToColor(150, 0, 255, 0, 0, 500); // Dark Purple
        Serial.println(F("Calibrating extrusion, Turning Leds low"));
        return;
    }

    // Homing toolhead
    if (printerVariables.stage == 13)
    {                                         // Homing toolhead
        tweenToColor(0, 175, 255, 0, 0, 500); // Snake Soft Blue
        Serial.println(F("Homing toolhead, Turning Leds low"));
        return;
    }

    // Cleaning nozzle
    if (printerVariables.stage == 14)
    {                                         // Cleaning nozzle
        tweenToColor(108, 255, 0, 0, 0, 500); // Soft Green
        Serial.println(F("Cleaning nozzle, Turning Leds low"));
        return;
    }

    // Heatbed preheating
    if (printerVariables.stage == 2)
    {                                         // Heatbed preheating
        tweenToColor(255, 172, 0, 0, 0, 500); // Soft Orange
        Serial.println(F("Heatbed preheating, Turning Leds low"));
        return;
    }

    // Auto Bed Leveling - BLINK
    if (printerVariables.stage == 1)
    {                                        // Auto Bed Leveling
        tweenToColor(0, 94, 255, 0, 0, 500); // Dark Blue
        delay(300);
        tweenToColor(0, 0, 0, 0, 0, 500); // OFF
        Serial.println(F("Auto Bed Leveling, Turning Leds low"));
        return;
    }

    // ON

    if (printerVariables.stage == 0)
    {                                         // Printing
        tweenToColor(0, 0, 0, 255, 255, 500); // ON
        Serial.println(F("Printing, Turning Leds On"));
        return;
    }

    if (printerVariables.stage == -1)
    {                                         // Idle
        tweenToColor(0, 0, 0, 255, 255, 500); // ON
        // Serial.println(F("Idle, Turning Leds On"));
        return;
    }

    if (printerVariables.stage == -2)
    {                                         // Offline
        tweenToColor(0, 0, 0, 255, 255, 500); // ON
        Serial.println(F("Stage -2, Turning Leds On"));
        return;
    }
}

void setupLeds()
{
    // pinMode(redPin, OUTPUT);
    // pinMode(greenPin, OUTPUT);
    // pinMode(bluePin, OUTPUT);
    // pinMode(coldPin, OUTPUT);
    // pinMode(warmPin, OUTPUT);
    strip.begin();
    strip.setBrightness(50);
    strip.show();
    updateleds();
}

void ledsloop()
{
    if ((millis() - printerVariables.finishstartms) >= 300000 && (printerVariables.gcodeState == "FINISH" || printerVariables.gcodeState == "FAILED"))
    {
        printerVariables.gcodeState = "IDLE";
        updateleds();
    }
    // snakeToColor(0, 175, 255, 0, 0, 50);
}

#endif