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
    int airQuality = -1;

    bool dhtValid = false;
    bool ens160Valid = false;
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

    uint8_t ens160Address = 0;
    bool ens160Available = false;

    SensorSample lastSample;

    mutable portMUX_TYPE sampleMux =
        portMUX_INITIALIZER_UNLOCKED;

    bool detectEns160();
    bool writeEns160Register(
        uint8_t reg,
        uint8_t value
    );
    bool readEns160Register(
        uint8_t reg,
        uint8_t& value
    );
};
