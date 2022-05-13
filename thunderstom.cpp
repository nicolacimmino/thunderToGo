#include "thunderstorm.h"

Thunderstorm::Thunderstorm()
{
    this->lightning = new SparkFun_AS3935(AS3935_ADDR);

    this->lightning->begin();
    this->lightning->resetSettings();
    this->lightning->setIndoorOutdoor(OUTDOOR);
    this->lightning->spikeRejection(0);
}

void Thunderstorm::loop()
{
    if (!this->isActive())
    {
        this->strikes = 0;
        this->interferers = 0;
    }
}
void Thunderstorm::strikeDetected()
{
    // Need to wait at least 2mS from interrupt before reading the interrupt register.
    delay(2);

    byte interruptVector = this->lightning->readInterruptReg();

    if (interruptVector == DISTURBER_INT)
    {
        this->interferers++;

        return;
    }

    if (interruptVector == LIGHTNING_INT)
    {
        this->strikes++;
        this->lastStrikeTime = millis();

        this->energy = this->lightning->lightningEnergy();
        this->distance = this->lightning->distanceToStorm();

        return;
    }
}

bool Thunderstorm::isActive()
{
    return this->lastStrikeTime < STORM_TIMEOUT_MS;
}
