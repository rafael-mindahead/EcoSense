#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "WifiService.h"

class PerformanceService
{
public:
    explicit PerformanceService(
        WifiService& wifi
    );

    void begin();

    void recordTiming(
        const String& name,
        uint32_t durationMicros
    );

    void addBusyMicros(
        uint32_t durationMicros
    );

    void registerTask(
        const String& name,
        TaskHandle_t handle
    );

    String snapshotJson();

private:
    struct TimingMetric
    {
        String name;
        uint32_t lastMicros = 0;
        uint32_t maxMicros = 0;
        uint64_t totalMicros = 0;
        uint32_t samples = 0;
    };

    struct TaskMetric
    {
        String name;
        TaskHandle_t handle = nullptr;
    };

    WifiService& wifi;

    TimingMetric timings[8];
    TaskMetric tasks[8];

    size_t timingCount = 0;
    size_t taskCount = 0;

    portMUX_TYPE mux =
        portMUX_INITIALIZER_UNLOCKED;

    uint64_t busyMicros = 0;
    uint64_t previousBusyMicros = 0;
    uint32_t previousCpuSampleMs = 0;
    float cpuUsagePercent = 0.0f;

    void updateCpuEstimate();
};
