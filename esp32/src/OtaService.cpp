#include "OtaService.h"

#include <Update.h>

OtaService::OtaService(
    LogService& logs
) :
    logs(logs)
{
}

bool OtaService::beginUpdate()
{
    errorState = false;
    started = false;
    completed = false;

    if (
        !Update.begin(
            UPDATE_SIZE_UNKNOWN
        )
    ) {
        errorState = true;

        logs.error(
            "OTA: falha ao iniciar update"
        );

        return false;
    }

    started = true;

    logs.info(
        "OTA: upload iniciado"
    );

    return true;
}

bool OtaService::writeChunk(
    uint8_t* data,
    size_t length
)
{
    if (errorState) {
        return false;
    }

    const size_t written =
        Update.write(
            data,
            length
        );

    if (written != length) {
        errorState = true;

        logs.error(
            "OTA: falha ao gravar bloco"
        );

        return false;
    }

    return true;
}

bool OtaService::finishUpdate()
{
    if (
        !started ||
        errorState ||
        !Update.end(true)
    ) {
        errorState = true;

        logs.error(
            "OTA: falha ao finalizar update"
        );

        return false;
    }

    completed = true;

    logs.info(
        "OTA: firmware atualizado"
    );

    return true;
}

bool OtaService::hasError() const
{
    return errorState;
}

bool OtaService::isComplete() const
{
    return
        started &&
        completed &&
        !errorState;
}
