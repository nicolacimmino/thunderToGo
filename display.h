
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <Adafruit_SSD1306.h>
#include "leds.h"
#include "thunderstorm.h"
#include "system.h"
#include "icons.h"

#define OLED_RESET -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DISPLAY_I2C_ADDRESS 0x3C
#define SSD1306_NO_SPLASH
#define DISPLAY_TIMEOUT_MS 10000

#define DISPLAY_MODE_MAIN 0
#define DISPLAY_MODE_BRIGHTNESS 1
#define DISPLAY_MODE_INOUTDOOR 2
#define DISPLAY_MODE_REJECTSPIKES 3
#define DISPLAY_MODE_TEST 4
#define DISPLAY_MODE_STATS 5
#define DISPLAY_MODES 6

#define MAX_BRIGHTNESS 4

class Display
{
private:
    Thunderstorm *thunderstorm;
    Leds *leds;
    bool awake = true;
    unsigned long idleSince = 0;
    Adafruit_SSD1306 *oled;
    char buffer[160];
    unsigned long lastScreenRefresh = 0;
    uint8_t mode = 0;

    void printHeader();
    void keepAwake();
    void loopMainMode();
    void loopBrightness();
    void loopStatsMode();
    void loopTestMode();
    void loopInOutdoor();
    void loopRejectSpikes();

    void writeCentered(uint8_t y);
    void writeRight(uint8_t y);
    void write(uint8_t x, uint8_t y);
    
public:
    Display(Thunderstorm *thunderstorm, Leds *leds);
    void loop(bool forceRefresh);
    void onClick();
    void onLongPress();    
};

#endif