#!/bin/bash

set -e

echo "→ Publicando telemetria MQTT..."

docker compose exec -T mosquitto \
    mosquitto_pub \
    -h localhost \
    -p 1883 \
    -t "ecosense/device/1/telemetry" \
    -m '{
        "temperature": 27.5,
        "humidity": 62,
        "luminosity": 750,
        "air_quality": 90
    }'

sleep 2

RESULT=$(docker compose exec -T postgres sh -c \
'psql -U "$POSTGRES_USER" \
-d "$POSTGRES_DB" \
-t -c "
SELECT COUNT(*)
FROM meansurements
WHERE temperature = 27.5;
"')

RESULT=$(echo "$RESULT" | tr -d '[:space:]')

if [ "$RESULT" -lt 1 ]; then
    echo "❌ Telemetria MQTT não chegou ao banco"
    exit 1
fi

echo "✅ MQTT → Consumer → PostgreSQL funcionando"