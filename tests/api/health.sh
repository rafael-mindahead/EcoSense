#!/bin/bash

set -e

BASE_URL="http://localhost:8080"

echo "→ Testando API health..."

STATUS=$(curl -s \
    -o /tmp/ecosense_health.json \
    -w "%{http_code}" \
    "$BASE_URL/api/health")

if [ "$STATUS" != "200" ]; then
    echo "❌ Health falhou: HTTP $STATUS"
    cat /tmp/ecosense_health.json
    exit 1
fi

echo "✅ API online"