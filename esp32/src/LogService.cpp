#include "LogService.h"

#include <LittleFS.h>
#include <vector>

#include "AppConfig.h"

namespace {
constexpr char LOG_FILE[] =
    "/ecosense.log";

constexpr char LOG_TEMP_FILE[] =
    "/ecosense.log.tmp";
}

LogService::LogService(
    TimeService& time
) :
    time(time)
{
}

bool LogService::begin()
{
    mutex =
        xSemaphoreCreateMutex();

    if (
        mutex == nullptr ||
        !LittleFS.begin(true)
    ) {
        return false;
    }

    entries =
        countLinesUnlocked();

    return true;
}

void LogService::info(
    const String& message
)
{
    write(
        LogLevel::Info,
        message
    );
}

void LogService::warning(
    const String& message
)
{
    write(
        LogLevel::Warning,
        message
    );
}

void LogService::error(
    const String& message
)
{
    write(
        LogLevel::Error,
        message
    );
}

void LogService::write(
    LogLevel level,
    const String& message
)
{
    if (
        mutex == nullptr ||
        xSemaphoreTake(
            mutex,
            pdMS_TO_TICKS(1000)
        ) != pdTRUE
    ) {
        return;
    }

    File file =
        LittleFS.open(
            LOG_FILE,
            FILE_APPEND
        );

    if (!file) {
        xSemaphoreGive(mutex);

        Serial.println(
            "Falha ao abrir log."
        );

        return;
    }

    file.print(
        time.timestamp()
    );

    file.print("|");

    file.print(
        levelName(level)
    );

    file.print("|");

    file.println(message);

    file.close();

    entries++;

    if (
        entries >
        EcoSenseConfig::
            MAX_LOG_ENTRIES
    ) {
        trimToLastUnlocked(
            EcoSenseConfig::
                MAX_LOG_ENTRIES
        );
    }

    xSemaphoreGive(mutex);
}

const char* LogService::levelName(
    LogLevel level
) const
{
    switch (level) {
        case LogLevel::Info:
            return "INFO";

        case LogLevel::Warning:
            return "WARNING";

        case LogLevel::Error:
            return "ERROR";
    }

    return "INFO";
}

size_t LogService::
    countLinesUnlocked() const
{
    File file =
        LittleFS.open(
            LOG_FILE,
            FILE_READ
        );

    if (!file) {
        return 0;
    }

    size_t count = 0;

    while (file.available()) {
        const String line =
            file.readStringUntil('\n');

        if (!line.isEmpty()) {
            count++;
        }
    }

    file.close();

    return count;
}

void LogService::
    trimToLastUnlocked(
        size_t keepEntries
    )
{
    const size_t current =
        countLinesUnlocked();

    if (current <= keepEntries) {
        entries = current;
        return;
    }

    const size_t skip =
        current - keepEntries;

    File source =
        LittleFS.open(
            LOG_FILE,
            FILE_READ
        );

    if (!source) {
        return;
    }

    File target =
        LittleFS.open(
            LOG_TEMP_FILE,
            FILE_WRITE
        );

    if (!target) {
        source.close();
        return;
    }

    size_t index = 0;

    while (source.available()) {
        String line =
            source.readStringUntil('\n');

        if (index++ >= skip) {
            target.println(line);
        }
    }

    source.close();
    target.close();

    LittleFS.remove(LOG_FILE);

    LittleFS.rename(
        LOG_TEMP_FILE,
        LOG_FILE
    );

    entries = keepEntries;
}

String LogService::readTail(
    size_t maxEntries
) const
{
    if (
        mutex == nullptr ||
        xSemaphoreTake(
            mutex,
            pdMS_TO_TICKS(1000)
        ) != pdTRUE
    ) {
        return "";
    }

    File file =
        LittleFS.open(
            LOG_FILE,
            FILE_READ
        );

    if (!file) {
        xSemaphoreGive(mutex);
        return "";
    }

    std::vector<String> lines;
    lines.reserve(maxEntries);

    while (file.available()) {
        String line =
            file.readStringUntil('\n');

        if (line.isEmpty()) {
            continue;
        }

        if (
            lines.size() >=
            maxEntries
        ) {
            lines.erase(
                lines.begin()
            );
        }

        lines.push_back(line);
    }

    file.close();

    String output;

    for (
        const String& line :
        lines
    ) {
        output += line;
        output += "\n";
    }

    xSemaphoreGive(mutex);

    return output;
}

const char* LogService::filePath() const
{
    return LOG_FILE;
}
