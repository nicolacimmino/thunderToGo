
#include "leds.h"

Leds::Leds(Thunderstorm *thunderstorm)
{
    this->thunderstorm = thunderstorm;

    FastLED.addLeds<WS2812B, 5, GRB>(led, LEDS_COUNT);
    FastLED.setBrightness(255);

    for (int ix = 0; ix < LEDS_COUNT; ix++)
    {
        this->led[ix] = CRGB::Black;
    }
}

void Leds::loop()
{
    uint16_t interval = (this->thunderstorm->isActive() ? 500 : 2000);

    if (!this->thunderstorm->isSensorActive())
    {
        interval = 500;
    }

    for (int ix = 0; ix < LEDS_COUNT; ix++)
    {
        if (millis() % interval < 100)
        {
            if (!this->thunderstorm->isSensorActive())
            {
                led[ix] = CRGB::Blue;
                continue;
            }

            led[ix] = this->thunderstorm->isActive() ? CRGB::Red : CRGB::Green;
        }
        else
        {
            led[ix] = CRGB::Black;
        }
    }

    FastLED.setBrightness(this->highBrightness ? 255 : 40);

    FastLED.show();
}

void Leds::lightningShow()
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