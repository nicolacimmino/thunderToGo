#include "display.h"

Display::Display(Thunderstorm *thunderstorm)
{
    this->thunderstorm = thunderstorm;

    this->oled = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    this->oled->begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS);
    this->oled->clearDisplay();
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
    char message[160];

    //if (!thunderstormActive)
    // if (1)
    // {
    //     strcpy(message, "        Thunder  \n"
    //                     "                   \n"
    //                     "STK: ---            \n"
    //                     "DST: ---            \n"
    //                     "ENE: ---            \n"
    //                     "TMS: ---            \n"
    //                     "                   \n");
    // }
    // else
    // {

    uint16_t timeInMinutes = (this->thunderstorm->lastStrikeTime - millis()) / 60000;

    sprintf(message, "        Thunder  \n"
                     "                   \n"
                     "STK: %d            \n"
                     "DST: %d            \n"
                     "ENE: %d            \n"
                     "TMS: %d            \n"
                     "INT: %d            \n",
            this->thunderstorm->strikes, this->thunderstorm->distance, this->thunderstorm->energy, 0, this->thunderstorm->interferers);

    static uint16_t lastPoorCrc = 0;

    uint16_t poorCrc = 0;
    for (uint8_t ix = 0; ix < strlen(message); ix++)
    {
        poorCrc += message[ix];
    }

    if (poorCrc != lastPoorCrc)
    {
        this->oled->dim(0);
        this->oled->clearDisplay();
        this->oled->setTextSize(1);              // Normal 1:1 pixel scale
        this->oled->setTextColor(SSD1306_WHITE); // Draw white text
        this->oled->cp437(true);                 // Use full 256 char 'Code Page 437' font
        this->oled->setCursor(0, 1);
        this->oled->write(message);
        this->oled->display();

        lastPoorCrc = poorCrc;
    }
}
