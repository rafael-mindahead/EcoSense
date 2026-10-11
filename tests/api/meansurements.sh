#!/bin/bash

set -e

BASE_URL="http://localhost:8080"

echo "→ Testando medição válida..."

STATUS=$(curl -s \
    -o /tmp/ecosense_measurement.json \
    -w "%{http_code}" \
    -X POST \
    "$BASE_URL/api/v1/meansurements" \
    -H "Content-Type: application/json" \
    -d '{
        "device_id": 1,
        "temperature": 25,
        "humidity": 60,
        "luminosity": 700,
        "air_quality": 90
    }')

if [ "$STATUS" != "201" ]; then
    echo "❌ Esperado 201, recebido $STATUS"
    cat /tmp/ecosense_measurement.json
    exit 1
fi

echo "✅ Medição válida aceita"

echo "→ Testando validação..."

STATUS=$(curl -s \
    -o /tmp/ecosense_invalid.json \
    -w "%{http_code}" \
    -X POST \
    "$BASE_URL/api/v1/meansurements" \
    -H "Content-Type: application/json" \
    -d '{
        "device_id": 1,
        "temperature": 25,
        "humidity": 900,
        "luminosity": -50,
        "air_quality": -10
    }')

if [ "$STATUS" != "422" ]; then
    echo "❌ Esperado 422, recebido $STATUS"
    cat /tmp/ecosense_invalid.json
    exit 1
fi

echo "✅ Validação rejeitou valores inválidos"