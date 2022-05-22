#include "thunderstorm.h"

Thunderstorm::Thunderstorm()
{
    this->lightning = new SparkFun_AS3935(AS3935_ADDR);

    this->lightning->begin();

    // Why not masking disturbers? We use this as a way to ensure the sensor
    //  is connected and functioning. There's a piece of code that alerts if
    //  nothing has been received from the sensor (including distrbers) for
    //  a certain time.
    // TODO: this has been dropped, needs anyway to be replaced by a test mode.
    this->lightning->maskDisturber(0);
}

void Thunderstorm::loop()
{
    if (!this->isActive())
    {
        this->strikes = 0;
        this->distance = 0;
        this->lastStrikeTime = 0;
    }
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