#pragma once

#include <Arduino.h>

#include "LogService.h"

class OtaService
{
public:
    explicit OtaService(
        LogService& logs
    );

    bool beginUpdate();
    bool writeChunk(
        uint8_t* data,
        size_t length
    );
    bool finishUpdate();

    bool hasError() const;
    bool isComplete() const;

private:
    LogService& logs;
    bool errorState = false;
    bool started = false;
    bool completed = false;
};
