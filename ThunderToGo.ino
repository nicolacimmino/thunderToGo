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
#include <FastLED.h>
#include <avr/wdt.h>

#define LEDS_COUNT 1
#define PIN_BUTTON 7
#define PIN_THUNDER_IRQ 0

#include "display.h"
#include "thunderstorm.h"

Display *display;
Thunderstorm *thunderstorm;

bool showOngoing = false;

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

    //Setup a watchdog interrupt every 64mS.
    cli();
    _WD_CONTROL_REG = (1 << WDCE) | (1 << WDE);
    _WD_CONTROL_REG = (1 << WDIE) | (1 << WDP1);
    sei();

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonISR, FALLING);

    pinMode(PIN_THUNDER_IRQ, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_THUNDER_IRQ), thunderISR, RISING);
}

ISR(WDT_vect)
{
    if (showOngoing)
    {
        return;
    }

    uint8_t timeSinceLastStrikeMinutes = floor(((millis() - thunderstorm->lastStrikeTime) / 60000));

    float breathRate = (thunderstorm->isActive() && timeSinceLastStrikeMinutes < 5) ? (2000.0 / (float)(min(thunderstorm->strikes, 10))) : 2000.0;

    // Keep breathing! See Sean Voisen great post from which I grabbed the formula.
    // https://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
    float val = (exp(sin(millis() / breathRate * PI)) - 0.36787944) * 108.0;

    if (!thunderstorm->isActive())
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

void loop()
{
    if (buttonInterrupt)
    {
        display->keepAwake();
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
}
