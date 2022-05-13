

//  Copyright (C) 2022 Nicola Cimmino
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include <SPI.h>
#include <Wire.h>
#include "SparkFun_AS3935.h"
#include <FastLED.h>
#include <avr/wdt.h>

#define AS3935_ADDR 0x03
#define INDOOR 0x12
#define OUTDOOR 0xE
#define LIGHTNING_INT 0x08
#define DISTURBER_INT 0x04
#define NOISE_INT 0x01
#define LEDS_COUNT 1
#define PIN_BUTTON 7

#include "display.h"

SparkFun_AS3935 lightning(AS3935_ADDR);
Display *display;

CRGB led[LEDS_COUNT];

int strikes = 0;
int distance = 0;
int energy = 0;
int interferers = 0;
unsigned long lastStrikeTime = 0;
int timeSinceLastStrikeMinutes = 0;
bool thunderstormActive = false;
bool showOngoing = false;

bool buttonInterrupt = false;


void buttonISR()
{
    buttonInterrupt = true;
}

void setup()
{
    display = new Display();

    FastLED.addLeds<WS2812B, 5, GRB>(led, LEDS_COUNT);
    FastLED.setBrightness(255);

    Wire.begin();
    lightning.begin();
    lightning.resetSettings();
    lightning.setIndoorOutdoor(INDOOR);
    lightning.spikeRejection(2);

    for (int ix = 0; ix < LEDS_COUNT; ix++)
    {
        led[ix] = CRGB::Black;
    }

    //Setup a watchdog interrupt every 64mS.
    cli();
    _WD_CONTROL_REG = (1 << WDCE) | (1 << WDE);
    _WD_CONTROL_REG = (1 << WDIE) | (1 << WDP1);
    sei();

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonISR, FALLING);
}

ISR(WDT_vect)
{
    if (showOngoing)
    {
        return;
    }

    timeSinceLastStrikeMinutes = floor(((millis() - lastStrikeTime) / 60000));

    float breathRate = (thunderstormActive && timeSinceLastStrikeMinutes < 5) ? (2000.0 / (float)(min(strikes, 10))) : 2000.0;

    // Keep breathing! See Sean Voisen great post from which I grabbed the formula.
    // https://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
    float val = (exp(sin(millis() / breathRate * PI)) - 0.36787944) * 108.0;

    if (!thunderstormActive)
    {
        CRGB noStormColor = CRGB::DarkGreen;
        noStormColor.fadeToBlackBy(255 - val);

        for (int ix = 0; ix < LEDS_COUNT; ix++)
        {
            led[ix] = noStormColor;
        }

        FastLED.show();

        return;
    }

    if (timeSinceLastStrikeMinutes > 90)
    {
        strikes = 0;
        thunderstormActive = false;

        return;
    }

    CRGB color = CHSV(timeSinceLastStrikeMinutes, 255, 255);
    color.fadeToBlackBy(255 - val);

    for (int ix = 0; ix < LEDS_COUNT; ix++)
    {
        led[ix] = color;
    }
    FastLED.show();
}

void lightningShow()
{
    showOngoing = true;

    byte previousBrightness = FastLED.getBrightness();

    for (int ix = 0; ix < LEDS_COUNT; ix++)
    {
        led[ix] = CRGB::White;
    }

    for (int l = 0; l < random(4, 12); l++)
    {
        FastLED.setBrightness(random(20, 255));
        FastLED.show();
        delay(random(1, 50));

        FastLED.setBrightness(0);
        FastLED.show();
        delay(random(1, 150));
    }

    FastLED.setBrightness(previousBrightness);

    for (int ix = 0; ix < LEDS_COUNT; ix++)
    {
        led[ix] = CRGB::Black;
    }
    FastLED.show();

    showOngoing = false;
}

void readSensor()
{
    byte intVal = lightning.readInterruptReg();
    if (intVal)
    {
        if (intVal == DISTURBER_INT)
        {
            interferers++;
        }
        else if (intVal == LIGHTNING_INT)
        {
            strikes++;
            energy = lightning.lightningEnergy();
            distance = lightning.distanceToStorm();

            lastStrikeTime = millis();
            lightningShow();
            thunderstormActive = true;
        }

        while (lightning.readInterruptReg())
        {
            delay(1);
        }
    }
}

void loop()
{
    readSensor();
    display->loop();

    if (buttonInterrupt)
    {
        display->keepAwake();
        buttonInterrupt = false;
    }
}
