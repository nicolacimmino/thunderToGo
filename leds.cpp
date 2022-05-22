
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

CRGB Leds::getStormColor()
{
    if (!this->thunderstorm->isActive())
    {
        return CRGB::Green;
    }

    // Red Hue=0, Green Hue=96. In 15min => 6.4/min
    return CRGB(CHSV(this->thunderstorm->minutesSinceLastStrike() * 6.4, 255, 255));
}

uint16_t Leds::getFlashInterval()
{
    if (!this->thunderstorm->isActive())
    {
        return 2000;
    }

    // 0km=> 200mS / 50km => 700mS
    return 200 + (this->thunderstorm->distance * 10);
}

void Leds::loop()
{
    for (int ix = 0; ix < LEDS_COUNT; ix++)
    {
        if (millis() % this->getFlashInterval() < 100)
        {
            led[ix] = this->getStormColor();
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