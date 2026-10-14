#pragma once

#include <Arduino.h>

#include "TimeService.h"

enum class LogLevel
{
    Info,
    Warning,
    Error
};

class LogService
{
public:
    explicit LogService(
        TimeService& time
    );

    bool begin();

    void info(
        const String& message
    );

    void warning(
        const String& message
    );

    void error(
        const String& message
    );

    String readTail(
        size_t maxEntries = 100
    ) const;

    const char* filePath() const;

private:
    TimeService& time;
    size_t entries = 0;

    void write(
        LogLevel level,
        const String& message
    );

    const char* levelName(
        LogLevel level
    ) const;

    size_t countLines() const;
    void trimToLast(
        size_t keepEntries
    );
};
