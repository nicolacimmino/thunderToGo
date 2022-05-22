#include "display.h"

Display::Display(Thunderstorm *thunderstorm, Leds *leds)
{
    this->thunderstorm = thunderstorm;
    this->leds = leds;

    this->oled = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    this->oled->begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS);
    this->oled->dim(0);
    this->oled->clearDisplay();
    this->oled->setTextSize(1);              // Normal 1:1 pixel scale
    this->oled->setTextColor(SSD1306_WHITE); // Draw white text
    this->oled->cp437(true);                 // Use full 256 char 'Code Page 437' font
}

void Display::loop(bool forceRefresh)
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

    if (!forceRefresh && this->lastScreenRefresh != 0 && millis() - this->lastScreenRefresh < 1000)
    {
        return;
    }

    this->lastScreenRefresh = millis();

    this->oled->clearDisplay();
    this->printHeader();

    switch (this->mode)
    {
    case DISPLAY_MODE_MAIN:
        this->loopMainMode();
        break;
    case DISPLAY_MODE_BRIGHTNESS:
        this->loopBrightness();
        break;
    case DISPLAY_MODE_STATS:
        this->loopStatsMode();
        break;
    case DISPLAY_MODE_INOUTDOOR:
        this->loopInOutdoor();
        break;
    case DISPLAY_MODE_REJECTSPIKES:
        this->loopRejectSpikes();
        break;
    }

    this->oled->display();
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
    this->loop(true);
}

void Display::onLongPress()
{
    switch (this->mode)
    {
    case DISPLAY_MODE_BRIGHTNESS:
        this->leds->highBrightness = !this->leds->highBrightness;
        break;
    case DISPLAY_MODE_INOUTDOOR:
        this->thunderstorm->changeMode();
        break;
    case DISPLAY_MODE_REJECTSPIKES:
        this->thunderstorm->increaseRejectSpikes();
        break;
    }

    this->keepAwake();
    this->loop(true);
}

void Display::printHeader()
{
    sprintf(this->buffer, "%d %%", getBatteryLevel());
    this->writeRight(0);

    if (this->thunderstorm->isIndoorMode())
    {
        sprintf(this->buffer, "INDOOR");
    }
    else
    {
        sprintf(this->buffer, "OUTDOOR");
    }

    this->write(0, 0);
}

void Display::loopMainMode()
{
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
}

void Display::loopStatsMode()
{
    sprintf(this->buffer, "Registers");
    this->writeCentered(16);

    sprintf(this->buffer, "STK: %04d   DST: %04d", this->thunderstorm->strikes, this->thunderstorm->distance);
    this->write(0, 32);

    sprintf(this->buffer, "TMS: %04d   SNS: %04d", this->thunderstorm->minutesSinceLastStrike(), this->thunderstorm->minutesSinceLastSensorEvent());
    this->write(0, 42);

    sprintf(this->buffer, "BAT: %04d   UPT: %04d", measuredVcc, (uint16_t)floor(millis() / 60000));
    this->write(0, 52);
}

void Display::loopInOutdoor()
{
    this->oled->setTextSize(2);

    sprintf(this->buffer, "Mode");
    this->writeCentered(16);

    if (this->thunderstorm->isIndoorMode())
    {
        sprintf(this->buffer, "INDOOR");
    }
    else
    {
        sprintf(this->buffer, "OUTDOOR");
    }

    this->writeCentered(42);
    this->oled->setTextSize(1);
}

void Display::loopBrightness()
{
    this->oled->setTextSize(2);

    sprintf(this->buffer, "Brightness");
    this->writeCentered(16);

     if (this->leds->highBrightness)
    {
        sprintf(this->buffer, "HIGH");
    }
    else
    {
        sprintf(this->buffer, "LOW");
    }

    this->writeCentered(42);
    this->oled->setTextSize(1);
}

void Display::loopRejectSpikes()
{
    this->oled->setTextSize(2);

    sprintf(this->buffer, "Reject");
    this->writeCentered(16);

    sprintf(this->buffer, "%d", this->thunderstorm->getRejectSpikes());

    this->writeCentered(42);
    this->oled->setTextSize(1);
}

void Display::writeCentered(uint8_t y)
{
    int16_t x1;
    int16_t y1;
    uint16_t w;
    uint16_t h;

    this->oled->getTextBounds(this->buffer, 0, 0, &x1, &y1, &w, &h);
    this->oled->setCursor((SCREEN_WIDTH - w) / 2, y);
    this->oled->write(this->buffer);
}

void Display::writeRight(uint8_t y)
{
    int16_t x1;
    int16_t y1;
    uint16_t w;
    uint16_t h;

    this->oled->getTextBounds(this->buffer, 0, 0, &x1, &y1, &w, &h);
    this->oled->setCursor((SCREEN_WIDTH - w), y);
    this->oled->write(this->buffer);
}

void Display::write(uint8_t x, uint8_t y)
{
    this->oled->setCursor(x, y);
    this->oled->write(this->buffer);
}