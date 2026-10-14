#pragma once

#include <Arduino.h>

#include "TimeService.h"

struct SensorSample;

class HistoryService
{
public:
    explicit HistoryService(
        TimeService& time
    );

    bool begin();

    void append(
        const SensorSample& sample
    );

    String readLast24HoursJson(
        size_t maxPoints = 288
    ) const;

    const char* filePath() const;

private:
    TimeService& time;

    void trimIfNeeded();
};
