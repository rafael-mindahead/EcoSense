#pragma once

#include <Arduino.h>

class ActuatorHardwareService
{
public:
    void begin();

    bool execute(
        const String& actuator,
        const String& command,
        String& error
    );

    bool fanOn() const;
    bool exhaustOn() const;

private:
    bool fanState = false;
    bool exhaustState = false;

    void writeDriver(
        uint8_t pin,
        bool on
    );
};
