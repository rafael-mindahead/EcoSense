#!/bin/bash

set -e

BASE_URL="http://localhost:8080"

echo "→ Testando comando inválido..."

STATUS=$(curl -s \
    -o /tmp/ecosense_actuator_invalid.json \
    -w "%{http_code}" \
    -X POST \
    "$BASE_URL/api/v1/actuators/fan" \
    -H "Content-Type: application/json" \
    -d '{
        "device_id": 1,
        "command": "INVALID"
    }')

if [ "$STATUS" != "422" ]; then
    echo "❌ Esperado 422, recebido $STATUS"
    cat /tmp/ecosense_actuator_invalid.json
    exit 1
fi

echo "✅ Comando inválido rejeitado"


echo "→ Testando device inexistente..."

STATUS=$(curl -s \
    -o /tmp/ecosense_actuator_device.json \
    -w "%{http_code}" \
    -X POST \
    "$BASE_URL/api/v1/actuators/fan" \
    -H "Content-Type: application/json" \
    -d '{
        "device_id": 999999,
        "command": "ON"
    }')

if [ "$STATUS" != "404" ]; then
    echo "❌ Esperado 404, recebido $STATUS"
    cat /tmp/ecosense_actuator_device.json
    exit 1
fi

echo "✅ Device inexistente rejeitado"


echo "→ Criando comando válido..."

STATUS=$(curl -s \
    -o /tmp/ecosense_actuator.json \
    -w "%{http_code}" \
    -X POST \
    "$BASE_URL/api/v1/actuators/fan" \
    -H "Content-Type: application/json" \
    -d '{
        "device_id": 1,
        "command": "ON"
    }')

if [ "$STATUS" != "201" ]; then
    echo "❌ Esperado 201, recebido $STATUS"
    cat /tmp/ecosense_actuator.json
    exit 1
fi

COMMAND_ID=$(
    grep -o '"id":[0-9]*' \
    /tmp/ecosense_actuator.json |
    head -1 |
    cut -d: -f2
)

if [ -z "$COMMAND_ID" ]; then
    echo "❌ command_id não encontrado"
    cat /tmp/ecosense_actuator.json
    exit 1
fi

echo "✅ Comando criado: $COMMAND_ID"


echo "→ Verificando status sent..."

DB_STATUS=$(
    docker compose exec -T postgres sh -c \
    "psql -U \"\$POSTGRES_USER\" \
    -d \"\$POSTGRES_DB\" \
    -t -A \
    -c \"SELECT status
        FROM actuator_commands
        WHERE id = $COMMAND_ID;\"" |
    tr -d '[:space:]'
)

if [ "$DB_STATUS" != "sent" ]; then
    echo "❌ Esperado sent, recebido $DB_STATUS"
    exit 1
fi

echo "✅ Comando marcado como sent"


echo "→ Simulando ACK do ESP32..."

docker compose exec -T mosquitto \
    mosquitto_pub \
    -h localhost \
    -p 1883 \
    -t "ecosense/device/1/commands/ack" \
    -m "{\"command_id\":$COMMAND_ID}"

sleep 2


DB_STATUS=$(
    docker compose exec -T postgres sh -c \
    "psql -U \"\$POSTGRES_USER\" \
    -d \"\$POSTGRES_DB\" \
    -t -A \
    -c \"SELECT status
        FROM actuator_commands
        WHERE id = $COMMAND_ID;\"" |
    tr -d '[:space:]'
)

if [ "$DB_STATUS" != "executed" ]; then
    echo "❌ Esperado executed, recebido $DB_STATUS"
    exit 1
fi

echo "✅ ACK alterou sent → executed"


echo "→ Testando ACK duplicado..."

RESTARTS_BEFORE=$(
    docker inspect -f '{{.RestartCount}}' \
    ecosense_mqtt_consumer
)

docker compose exec -T mosquitto \
    mosquitto_pub \
    -h localhost \
    -p 1883 \
    -t "ecosense/device/1/commands/ack" \
    -m "{\"command_id\":$COMMAND_ID}"

sleep 2

RESTARTS_AFTER=$(
    docker inspect -f '{{.RestartCount}}' \
    ecosense_mqtt_consumer
)

if [ "$RESTARTS_AFTER" != "$RESTARTS_BEFORE" ]; then
    echo "❌ ACK duplicado derrubou o consumer"
    exit 1
fi

echo "✅ ACK duplicado tratado sem derrubar consumer"