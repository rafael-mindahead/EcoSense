#!/bin/bash

set -e

BASE_URL="http://localhost:8080"

echo "→ Testando listagem de alertas..."

STATUS=$(curl -s \
    -o /tmp/ecosense_alerts.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/alerts")

if [ "$STATUS" != "200" ]; then
    echo "❌ Esperado 200, recebido $STATUS"
    cat /tmp/ecosense_alerts.json
    exit 1
fi

echo "✅ Listagem de alertas funcionando"

echo "→ Testando resolução com id inválido..."

STATUS=$(curl -s \
    -o /tmp/ecosense_alert_invalid.json \
    -w "%{http_code}" \
    -X POST \
    "$BASE_URL/api/v1/alerts/resolve" \
    -H "Content-Type: application/json" \
    -d '{
        "alert_id": -1
    }')

if [ "$STATUS" != "422" ]; then
    echo "❌ Esperado 422, recebido $STATUS"
    cat /tmp/ecosense_alert_invalid.json
    exit 1
fi

echo "✅ Validação de alert_id funcionando"
