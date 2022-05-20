#include "system.h"

long measuredVcc = 0;

uint8_t getBatteryLevel()
{
    // See this article for an in-depth explanation.
    // https://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
    // tl;dr: we switch the ADC to measure the internal 1.1v reference using Vcc as reference, the rest is simple math.

    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);

    delay(2);
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC))
        ;

    measuredVcc = 1125300L / (ADCL | (ADCH << 8));
    analogReference(DEFAULT);

    return min(max((measuredVcc - 2700) / 7, 0), 100);
}
