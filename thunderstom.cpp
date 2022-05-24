#include "thunderstorm.h"

Thunderstorm::Thunderstorm()
{
    this->lightning = new SparkFun_AS3935(AS3935_ADDR);

    this->lightning->begin();

    this->lightning->maskDisturber(1);
}

void Thunderstorm::loop()
{
    if (!this->isActive())
    {
        this->clearStorm();
    }
}

void Thunderstorm::clearStorm()
{
    this->strikes = 0;
    this->distance = 0;
    this->lastStrikeTime = 0;
}

bool Thunderstorm::strikeDetected()
{
    this->lastSensorEventTime = millis();

    uint8_t interruptVector = this->lightning->readInterruptReg();

    if (interruptVector == LIGHTNING_INT)
    {
        this->strikes++;
        this->lastStrikeTime = millis();

        this->distance = this->lightning->distanceToStorm();

        return true;
    }

    // In test mode treat disturbers as strikes.
    if (this->testMode && interruptVector == DISTURBER_INT)
    {
        this->strikes++;
        this->lastStrikeTime = millis();

        this->distance = random(0, 39);

        return true;
    }

    return false;
}

bool Thunderstorm::isActive()
{
    return this->strikes > 0 && (millis() - this->lastStrikeTime) < STORM_TIMEOUT_MS;
}

uint8_t Thunderstorm::minutesSinceLastStrike()
{
    return this->lastStrikeTime > 0 ? ((millis() - this->lastStrikeTime) / 60000) : 0;
}

uint32_t Thunderstorm::minutesSinceLastSensorEvent()
{
    return (millis() - this->lastSensorEventTime) / 60000;
}

bool Thunderstorm::isIndoorMode()
{
    return this->lightning->readIndoorOutdoor() == INDOOR;
}

uint8_t Thunderstorm::getRejectSpikes()
{
    return this->lightning->readSpikeRejection();
}

void Thunderstorm::changeMode()
{
    if (this->isIndoorMode())
    {
        this->lightning->setIndoorOutdoor(OUTDOOR);
    }
    else
    {
        this->lightning->setIndoorOutdoor(INDOOR);
    }
}

void Thunderstorm::increaseRejectSpikes()
{
    this->lightning->spikeRejection(max(1, (this->getRejectSpikes() + 1) % MAX_SPIKE_REJECT));
}

void Thunderstorm::setTestMode(bool on)
{
    this->testMode = on;

    // Mask disturbers unless we are in test mode where disturbers
    //  are treated as strikes.
    this->lightning->maskDisturber(!this->testMode);

    this->clearStorm();
}

bool Thunderstorm::getTestMode()
{
    return this->testMode;
}