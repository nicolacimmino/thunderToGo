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

    this->reportStatus();
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

void Display::reportStatus()
{
    if (this->lastScreenRefresh != 0 && millis() - this->lastScreenRefresh < 1000)
    {
        return;
    }

    this->lastScreenRefresh = millis();

    uint16_t timeInMinutes = this->thunderstorm->lastStrikeTime > 0 ? ((millis() - this->thunderstorm->lastStrikeTime) / 60000) : 0;

    sprintf(this->buffer, "        Thunder  \n"
                          "                   \n"
                          "STK: %d            \n"
                          "DST: %d            \n"
                          "TMS: %d            \n"
                          "INT: %d            \n",
            this->thunderstorm->strikes, this->thunderstorm->distance, timeInMinutes, this->thunderstorm->interferers);

    this->oled->clearDisplay();
    this->oled->setCursor(0, 1);
    this->oled->write(this->buffer);
    this->oled->display();
}
