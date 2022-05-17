#include "thunderstorm.h"

Thunderstorm::Thunderstorm()
{
    this->lightning = new SparkFun_AS3935(AS3935_ADDR);

    this->lightning->begin();
    this->lightning->resetSettings();
    this->lightning->setIndoorOutdoor(INDOOR);
    this->lightning->spikeRejection(2);
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

bool Thunderstorm::isSensorActive()
{
    return (millis() - this->lastSensorEventTime) < SENSOR_MAX_INTERVAL_MS;
}

uint32_t Thunderstorm::secondsSinceLastSensorEvent()
{
    return (millis() - this->lastSensorEventTime) / 1000;
}
