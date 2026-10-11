#!/bin/bash

set -e

count_valid_telemetry() {
    docker compose exec -T postgres sh -c \
    'psql -U "$POSTGRES_USER" -d "$POSTGRES_DB" -t -c "
        SELECT COUNT(*)
        FROM meansurements
        WHERE temperature = 19.91
          AND humidity = 41.91
          AND luminosity = 9191
          AND air_quality = 91.91;
    "' | tr -d '[:space:]'
}

count_invalid_telemetry() {
    docker compose exec -T postgres sh -c \
    'psql -U "$POSTGRES_USER" -d "$POSTGRES_DB" -t -c "
        SELECT COUNT(*)
        FROM meansurements
        WHERE humidity = 999;
    "' | tr -d '[:space:]'
}

echo "→ Testando escrita única via MQTT..."

BEFORE=$(count_valid_telemetry)

docker compose exec -T mosquitto \
    mosquitto_pub \
    -h localhost \
    -p 1883 \
    -t "ecosense/device/1/telemetry" \
    -m '{
        "temperature": 19.91,
        "humidity": 41.91,
        "luminosity": 9191,
        "air_quality": 91.91
    }'

sleep 2

AFTER=$(count_valid_telemetry)
DELTA=$((AFTER - BEFORE))

if [ "$DELTA" -ne 1 ]; then
    echo "❌ Esperado 1 registro MQTT, foram criados $DELTA"
    exit 1
fi

echo "✅ MQTT criou exatamente uma medição"

echo "→ Testando rejeição de telemetria inválida..."

INVALID_BEFORE=$(count_invalid_telemetry)
RESTARTS_BEFORE=$(docker inspect -f '{{.RestartCount}}' ecosense_mqtt_consumer)

docker compose exec -T mosquitto \
    mosquitto_pub \
    -h localhost \
    -p 1883 \
    -t "ecosense/device/1/telemetry" \
    -m '{
        "temperature": 25,
        "humidity": 999,
        "luminosity": 700,
        "air_quality": 90
    }'

sleep 2

INVALID_AFTER=$(count_invalid_telemetry)
RESTARTS_AFTER=$(docker inspect -f '{{.RestartCount}}' ecosense_mqtt_consumer)

if [ "$INVALID_AFTER" -ne "$INVALID_BEFORE" ]; then
    echo "❌ Telemetria inválida foi persistida"
    exit 1
fi

if [ "$RESTARTS_AFTER" -ne "$RESTARTS_BEFORE" ]; then
    echo "❌ Consumer reiniciou ao processar telemetria inválida"
    exit 1
fi

echo "✅ Telemetria inválida foi rejeitada sem derrubar o consumer"
