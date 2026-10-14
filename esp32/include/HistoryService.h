#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

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

    mutable SemaphoreHandle_t mutex =
        nullptr;

    void trimIfNeededUnlocked();
};
