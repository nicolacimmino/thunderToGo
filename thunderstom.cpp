#include "thunderstorm.h"

Thunderstorm::Thunderstorm()
{
    this->lightning = new SparkFun_AS3935(AS3935_ADDR);

    this->lightning->begin();
    this->lightning->resetSettings();
    this->lightning->setIndoorOutdoor(INDOOR);
    this->lightning->spikeRejection(2);
    this->lightning->maskDisturber(1);
}

void Thunderstorm::loop()
{
    if (!this->isActive())
    {
        this->strikes = 0;
        this->distance = 0;
        this->energy = 0;
        this->lastStrikeTime = 0;
    }
}

bool Thunderstorm::strikeDetected()
{
    uint8_t interruptVector = this->lightning->readInterruptReg();

    if (interruptVector == DISTURBER_INT)
    {
        this->interferers++;

        return true;
    }

    if (interruptVector == LIGHTNING_INT)
    {
        this->strikes++;
        this->lastStrikeTime = millis();

        this->energy = this->lightning->lightningEnergy();
        this->distance = this->lightning->distanceToStorm();

        return true;
    }

    return false;
}

bool Thunderstorm::isActive()
{
    return this->strikes > 0 && (millis() - this->lastStrikeTime) < STORM_TIMEOUT_MS;
}
