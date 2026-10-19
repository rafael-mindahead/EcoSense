#pragma once

#include <Arduino.h>
#include <DHT.h>

#include "TimeService.h"

struct SensorSample
{
    time_t epoch = 0;
    uint32_t uptimeMs = 0;

    float temperature = NAN;
    float humidity = NAN;

    int luminosity = 0;
    int airQuality = 0;

    bool dhtValid = false;
};

class SensorService
{
public:
    explicit SensorService(
        TimeService& time
    );

    void begin();

    SensorSample read();

    SensorSample latest() const;

private:
    TimeService& time;
    DHT dht;

    SensorSample lastSample;

    mutable portMUX_TYPE sampleMux =
        portMUX_INITIALIZER_UNLOCKED;
};
