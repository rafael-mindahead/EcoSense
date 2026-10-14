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
    if (!LittleFS.begin(true)) {
        return false;
    }

    entries = countLines();

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
    File file =
        LittleFS.open(
            LOG_FILE,
            FILE_APPEND
        );

    if (!file) {
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
        trimToLast(
            EcoSenseConfig::
                MAX_LOG_ENTRIES
        );
    }
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

size_t LogService::countLines() const
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

void LogService::trimToLast(
    size_t keepEntries
)
{
    const size_t current =
        countLines();

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
    File file =
        LittleFS.open(
            LOG_FILE,
            FILE_READ
        );

    if (!file) {
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

    return output;
}

const char* LogService::filePath() const
{
    return LOG_FILE;
}
