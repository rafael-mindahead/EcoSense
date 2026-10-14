#include "PerformanceService.h"

#include <ArduinoJson.h>

PerformanceService::PerformanceService(
    WifiService& wifi
) :
    wifi(wifi)
{
}

void PerformanceService::begin()
{
    previousCpuSampleMs =
        millis();
}

void PerformanceService::recordTiming(
    const String& name,
    uint32_t durationMicros
)
{
    portENTER_CRITICAL(&mux);

    size_t index =
        timingCount;

    for (
        size_t i = 0;
        i < timingCount;
        i++
    ) {
        if (
            timings[i].name ==
            name
        ) {
            index = i;
            break;
        }
    }

    if (
        index == timingCount &&
        timingCount < 8
    ) {
        timings[index].name =
            name;

        timingCount++;
    }

    if (index < 8) {
        TimingMetric& metric =
            timings[index];

        metric.lastMicros =
            durationMicros;

        metric.maxMicros =
            max(
                metric.maxMicros,
                durationMicros
            );

        metric.totalMicros +=
            durationMicros;

        metric.samples++;
    }

    portEXIT_CRITICAL(&mux);
}

void PerformanceService::addBusyMicros(
    uint32_t durationMicros
)
{
    portENTER_CRITICAL(&mux);

    busyMicros +=
        durationMicros;

    portEXIT_CRITICAL(&mux);
}

void PerformanceService::registerTask(
    const String& name,
    TaskHandle_t handle
)
{
    if (
        handle == nullptr ||
        taskCount >= 8
    ) {
        return;
    }

    tasks[taskCount].name =
        name;

    tasks[taskCount].handle =
        handle;

    taskCount++;
}

void PerformanceService::updateCpuEstimate()
{
    const uint32_t now =
        millis();

    const uint32_t elapsedMs =
        now - previousCpuSampleMs;

    if (elapsedMs < 1000) {
        return;
    }

    uint64_t currentBusy;

    portENTER_CRITICAL(&mux);
    currentBusy = busyMicros;
    portEXIT_CRITICAL(&mux);

    const uint64_t deltaBusy =
        currentBusy -
        previousBusyMicros;

    const uint64_t availableMicros =
        static_cast<uint64_t>(
            elapsedMs
        ) *
        1000ULL *
        portNUM_PROCESSORS;

    if (availableMicros > 0) {
        cpuUsagePercent =
            min(
                100.0f,
                static_cast<float>(
                    deltaBusy
                ) *
                100.0f /
                static_cast<float>(
                    availableMicros
                )
            );
    }

    previousBusyMicros =
        currentBusy;

    previousCpuSampleMs =
        now;
}

String PerformanceService::snapshotJson()
{
    updateCpuEstimate();

    DynamicJsonDocument document(
        4096
    );

    document["cpu_usage_percent_estimated"] =
        cpuUsagePercent;

    document["cpu_frequency_mhz"] =
        ESP.getCpuFreqMHz();

    document["heap_total"] =
        ESP.getHeapSize();

    document["heap_free"] =
        ESP.getFreeHeap();

    document["heap_min_free"] =
        ESP.getMinFreeHeap();

    document["flash_bytes"] =
        ESP.getFlashChipSize();

    document["psram_available"] =
        psramFound();

    document["psram_total"] =
        psramFound()
            ? ESP.getPsramSize()
            : 0;

    document["psram_free"] =
        psramFound()
            ? ESP.getFreePsram()
            : 0;

    document["task_count"] =
        uxTaskGetNumberOfTasks();

    document["wifi_connected"] =
        wifi.isConnected();

    document["wifi_rssi_dbm"] =
        wifi.rssi();

    document["wifi_ssid"] =
        wifi.ssid();

    document["station_ip"] =
        wifi.stationIp();

    document["ap_active"] =
        wifi.isAccessPointActive();

    document["ap_ip"] =
        wifi.accessPointIp();

    JsonArray timingArray =
        document["function_timings"]
            .to<JsonArray>();

    portENTER_CRITICAL(&mux);

    for (
        size_t i = 0;
        i < timingCount;
        i++
    ) {
        const TimingMetric& metric =
            timings[i];

        JsonObject item =
            timingArray
                .add<JsonObject>();

        item["name"] =
            metric.name;

        item["last_us"] =
            metric.lastMicros;

        item["max_us"] =
            metric.maxMicros;

        item["avg_us"] =
            metric.samples > 0
                ? static_cast<uint32_t>(
                    metric.totalMicros /
                    metric.samples
                )
                : 0;
    }

    portEXIT_CRITICAL(&mux);

    JsonArray taskArray =
        document["tasks"]
            .to<JsonArray>();

    for (
        size_t i = 0;
        i < taskCount;
        i++
    ) {
        JsonObject task =
            taskArray.add<JsonObject>();

        task["name"] =
            tasks[i].name;

        task["stack_high_water_bytes"] =
            tasks[i].handle
                ? uxTaskGetStackHighWaterMark(
                    tasks[i].handle
                ) *
                    sizeof(StackType_t)
                : 0;
    }

    String json;

    serializeJson(
        document,
        json
    );

    return json;
}
