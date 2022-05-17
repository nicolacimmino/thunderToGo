#include "display.h"

Display::Display(Thunderstorm *thunderstorm)
{
    this->thunderstorm = thunderstorm;

    this->oled = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    this->oled->begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS);
    this->oled->dim(0);
    this->oled->clearDisplay();
    this->oled->setTextSize(1);              // Normal 1:1 pixel scale
    this->oled->setTextColor(SSD1306_WHITE); // Draw white text
    this->oled->cp437(true);                 // Use full 256 char 'Code Page 437' font
}

void Display::loop()
{
    if (!this->awake)
    {
        return;
    }

    if ((millis() - idleSince) > DISPLAY_TIMEOUT_MS)
    {
        this->oled->ssd1306_command(SSD1306_DISPLAYOFF);
        this->awake = false;

        return;
    }

    if (this->lastScreenRefresh != 0 && millis() - this->lastScreenRefresh < 1000)
    {
        return;
    }

    this->lastScreenRefresh = millis();

    switch (this->mode)
    {
    case DISPLAY_MODE_MAIN:
        this->loopMainMode();
        break;
    case DISPLAY_MODE_STATS:
        this->loopStatsMode();
        break;
    }
}

void Display::keepAwake()
{
    idleSince = millis();

    if (!this->awake)
    {
        this->oled->ssd1306_command(SSD1306_DISPLAYON);
        this->awake = true;
    }
}

void Display::onClick()
{
    if (this->awake)
    {
        this->mode = (this->mode + 1) % DISPLAY_MODES;
        this->lastScreenRefresh = 0;
    }

    this->keepAwake();
    this->loop();
}

void Display::printHeader()
{
    this->oled->setCursor(0, 1);
    this->oled->print(getBatteryLevel());
    this->oled->print("%");
}

void Display::loopMainMode()
{
    this->oled->clearDisplay();

    this->printHeader();

    if (!this->thunderstorm->isSensorActive())
    {
        this->oled->drawBitmap(
            (this->oled->width() - ICONS_WIDTH) / 2,
            16,
            brokenLinkIcon,
            ICONS_WIDTH,
            ICONS_HEIGHT,
            SSD1306_WHITE);

        this->oled->display();

        return;
    }

    if (!this->thunderstorm->isActive())
    {
        this->oled->drawBitmap(
            (this->oled->width() - ICONS_WIDTH) / 2,
            16,
            sunIcon,
            ICONS_WIDTH,
            ICONS_HEIGHT,
            SSD1306_WHITE);

        this->oled->display();

        return;
    }

    this->oled->drawBitmap(
        (this->oled->width() - ICONS_WIDTH) / 2,
        16,
        thunderIcon,
        ICONS_WIDTH,
        ICONS_HEIGHT,
        SSD1306_WHITE);

    this->oled->setTextColor(SSD1306_BLACK);
    this->oled->setCursor(53, 35);
    sprintf(this->buffer, "%04d", this->thunderstorm->strikes);
    this->oled->print(this->buffer);

    this->oled->setTextColor(SSD1306_WHITE);
    this->oled->setCursor(10, 20);
    this->oled->print(this->thunderstorm->distance);
    this->oled->print(" km");

    this->oled->setCursor(90, 20);
    this->oled->print(this->thunderstorm->minutesSinceLastStrike());
    this->oled->print(" min");

    this->oled->display();
}

void Display::loopStatsMode()
{
    sprintf(this->buffer, "    Registers      \n"
                          "STK: %d            \n"
                          "DST: %d            \n"
                          "TMS: %d            \n"
                          "SNS: %d            \n",
            this->thunderstorm->strikes, 
            this->thunderstorm->distance, 
            this->thunderstorm->minutesSinceLastStrike(),
            this->thunderstorm->secondsSinceLastSensorEvent());

    this->oled->clearDisplay();
    this->printHeader();
    this->oled->setCursor(0, 20);
    this->oled->write(this->buffer);
    this->oled->display();
}
