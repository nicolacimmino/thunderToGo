#ifndef __LEDS_H__
#define __LEDS_H__

#include <Arduino.h>
#include <FastLED.h>
#include "thunderstorm.h"

#define LEDS_COUNT 2

class Leds
{
    private:
        CRGB led[LEDS_COUNT];
        Thunderstorm *thunderstorm;
        
    public:
        Leds(Thunderstorm *thunderstorm);
        void loop();
        void lightningShow();
        bool highBrightness = true;        
};

#endif