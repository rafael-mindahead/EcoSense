#pragma once

#include <Arduino.h>

#include "WifiService.h"

class TimeService
{
public:
    explicit TimeService(
        WifiService& wifi
    );

    void tick();

    bool isSynchronized() const;
    String timestamp() const;
    time_t epoch() const;

private:
    WifiService& wifi;
    bool configured = false;
};
