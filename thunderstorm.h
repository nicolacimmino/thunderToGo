#ifndef __THUNDERSTORM_H__
#define __THUNDERSTORM_H__

#include "SparkFun_AS3935.h"

#define AS3935_ADDR 0x03
#define INDOOR 0x12
#define OUTDOOR 0xE
#define LIGHTNING_INT 0x08
#define DISTURBER_INT 0x04
#define NOISE_INT 0x01

#define MAX_SPIKE_REJECT 8

/* 15 minutes */
#define STORM_TIMEOUT_MS 900000

class Thunderstorm
{

private:
    SparkFun_AS3935 *lightning;
    unsigned long lastSensorEventTime = 0;
    unsigned long lastStrikeTime = 0;

public:
    uint16_t strikes = 0;
    uint8_t distance = 0;
    bool testMode = false;

    Thunderstorm();
    void loop();
    bool isActive();
    bool strikeDetected();
    uint8_t minutesSinceLastStrike();
    uint32_t minutesSinceLastSensorEvent();
    bool isIndoorMode();
    uint8_t getRejectSpikes();
    void changeMode();
    void increaseRejectSpikes();
};

#endif