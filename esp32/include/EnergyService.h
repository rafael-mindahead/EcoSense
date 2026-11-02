#pragma once

#include <Arduino.h>

#include "LogService.h"
#include "WifiService.h"

class EnergyService
{
public:
    EnergyService(
        WifiService& wifi,
        LogService& logs
    );

    bool requestLightSleep(
        uint32_t seconds
    );

    void tick();

    bool isPending() const;

private:
    WifiService& wifi;
    LogService& logs;

    bool pending = false;
    uint32_t sleepSeconds = 0;
    uint32_t executeAtMs = 0;
};
