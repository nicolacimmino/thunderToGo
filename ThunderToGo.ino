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

#define PIN_BUTTON 7
#define PIN_THUNDER_IRQ 0

#include "leds.h"
#include "display.h"
#include "thunderstorm.h"

Leds *leds;
Display *display;
Thunderstorm *thunderstorm;

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
    leds = new Leds(thunderstorm);
    display = new Display(thunderstorm, leds);

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonISR, FALLING);

    pinMode(PIN_THUNDER_IRQ, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_THUNDER_IRQ), thunderISR, RISING);
}

void serveButton()
{
    unsigned long pressStartTime = millis();

    while (digitalRead(PIN_BUTTON) == LOW && (millis() - pressStartTime) < 500)
    {
        delay(1);
    }

    if ((millis() - pressStartTime) < 400)
    {
        display->onClick();
    }
    else
    {
        display->onLongPress();
    }

    buttonInterrupt = false;
}
void loop()
{
    if (buttonInterrupt)
    {
        serveButton();
    }

    if (thunderInterrupt)
    {
        thunderInterrupt = false;

        if (thunderstorm->strikeDetected())
        {
            leds->lightningShow();
        }
    }

    leds->loop();
    thunderstorm->loop();
    display->loop(false);
}
