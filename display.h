
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <Adafruit_SSD1306.h>
#include "thunderstorm.h"

#define OLED_RESET -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DISPLAY_I2C_ADDRESS 0x3C
#define SSD1306_NO_SPLASH
#define DISPLAY_TIMEOUT_MS 5000

class Display
{
private:
    Thunderstorm *thunderstorm;
    bool awake = true;
    unsigned long idleSince = 0;
    Adafruit_SSD1306 *oled;
    void reportStatus();

public:
    Display(Thunderstorm *thunderstorm);
    void loop();
    void keepAwake();
};

#endif