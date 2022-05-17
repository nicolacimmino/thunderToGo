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

#include <Wire.h>
#include <FastLED.h>

#define LEDS_COUNT 1
#define PIN_BUTTON 7
#define PIN_THUNDER_IRQ 0

#include "display.h"
#include "thunderstorm.h"

Display *display;
Thunderstorm *thunderstorm;

CRGB led[LEDS_COUNT];

bool buttonInterrupt = false;
bool thunderInterrupt = false;

void buttonISR()
{
    buttonInterrupt = true;
}

void thunderISR()
{
    thunderInterrupt = true;
}

void setup()
{
    Wire.begin();

    thunderstorm = new Thunderstorm();
    display = new Display(thunderstorm);

    FastLED.addLeds<WS2812B, 5, GRB>(led, LEDS_COUNT);
    FastLED.setBrightness(255);

    for (int ix = 0; ix < LEDS_COUNT; ix++)
    {
        led[ix] = CRGB::Black;
    }

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonISR, FALLING);

    pinMode(PIN_THUNDER_IRQ, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_THUNDER_IRQ), thunderISR, RISING);
}

void lightningShow()
{
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
}

void loop()
{
    if (buttonInterrupt)
    {
        display->onClick();
        buttonInterrupt = false;
    }

    if (thunderInterrupt)
    {
        thunderInterrupt = false;

        if (thunderstorm->strikeDetected())
        {
            lightningShow();
        }
    }

    thunderstorm->loop();
    display->loop();

    for (int ix = 0; ix < LEDS_COUNT; ix++)
    {
        if (millis() % (thunderstorm->isActive() ? 1000 : 2000) < 100)
        {
            led[ix] = thunderstorm->isActive() ? CRGB::Red : CRGB::Green;
        }
        else
        {
            led[ix] = CRGB::Black;
        }
    }

    FastLED.show();
}
