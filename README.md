# EcoSense

EcoSense é um sistema ciber-físico de monitoramento ambiental desenvolvido com ESP32, MQTT, PHP, PostgreSQL, Docker e interfaces Web/iOS.

O projeto monitora variáveis ambientais, armazena medições, avalia limites configurados, gera alertas e envia comandos para atuadores como ventiladores e exaustores.

> Status: em desenvolvimento. A infraestrutura, a API REST, o fluxo MQTT, os alertas e o controle de atuadores já estão implementados. A integração com o ESP32 físico depende da conexão USB adequada e será retomada em seguida.

## Objetivo

O EcoSense foi pensado para ambientes que precisam de acompanhamento contínuo, como estufas, laboratórios e espaços controlados.

A solução foi projetada para:

- monitorar temperatura e umidade;
- monitorar luminosidade;
- monitorar qualidade do ar;
- manter histórico de medições;
- acompanhar o estado dos dispositivos;
- gerar alertas a partir de limites ambientais;
- enviar comandos para ventiladores e exaustores;
- disponibilizar dados por API REST;
- exibir informações em dashboard Web;
- disponibilizar uma aplicação iOS em SwiftUI.

## Arquitetura atual

### Telemetria

```text
ESP32
  ↓ Wi-Fi / MQTT
Mosquitto
  ↓
MQTT Consumer (PHP)
  ↓
MeansurementService
  ├── MeansurementRepository
  ├── DeviceRepository
  ├── SensorLimitRepository
  └── AlertRepository
  ↓
PostgreSQL
  ↓
REST API
  ↓
Web / iOS
```

### Controle de atuadores

```text
Web / iOS
  ↓ REST
PHP API
  ↓
MqttService
  ↓ MQTT
Mosquitto
  ↓
ESP32
  ↓
Relé
  ↓
Ventilador / Exaustor
```

O ciclo de comando também possui confirmação por ACK:

```text
pending → sent → executed
```

## Tecnologias

### Embarcado

- ESP32
- C++
- Wi-Fi
- MQTT

### Backend

- PHP 8.3
- API REST
- Composer
- PSR-4

### Banco de dados

- PostgreSQL 17

### Mensageria IoT

- MQTT
- Eclipse Mosquitto
- php-mqtt/client

### Front-end planejado

- HTML
- CSS
- JavaScript
- Chart.js

### Mobile planejado

- Swift
- SwiftUI
- iOS

### Infraestrutura

- Docker
- Docker Compose
- Nginx
- PHP-FPM

## Estrutura principal

```text
EcoSense/
├── api/
│   ├── app/
│   │   ├── Config/
│   │   ├── Controllers/
│   │   ├── Core/
│   │   ├── Repositories/
│   │   └── Services/
│   ├── public/
│   ├── routes/
│   └── workers/
├── database/
│   └── migrations/
├── docker/
│   ├── mosquitto/
│   ├── nginx/
│   └── php/
├── Docs/
├── .env.example
├── docker-compose.yml
└── README.md
```

## Banco de dados

Entidades implementadas:

- `devices`
- `meansurements`
- `actuator_commands`
- `sensor_limits`
- `alerts`

O projeto mantém a grafia `meansurements` na API e no banco por compatibilidade com a implementação atual.

## MQTT

Tópicos atualmente utilizados:

```text
ecosense/device/+/telemetry
ecosense/device/+/commands/ack
ecosense/device/{id}/commands/fan
ecosense/device/{id}/commands/exhaust
```

Exemplo de telemetria:

```json
{
  "temperature": 27.1,
  "humidity": 63.2,
  "luminosity": 820,
  "air_quality": 94
}
```

## API REST

Endpoints implementados atualmente:

| Método | Endpoint | Descrição |
|---|---|---|
| GET | `/api/health` | Verifica o estado da API |
| GET | `/api/database/health` | Verifica a conexão com PostgreSQL |
| GET | `/api/v1/meansurements/latest` | Retorna a última medição |
| GET | `/api/v1/meansurements?limit=50` | Retorna o histórico de medições |
| POST | `/api/v1/meansurements` | Registra uma medição e processa alertas |
| GET | `/api/v1/devices` | Lista os dispositivos |
| POST | `/api/v1/actuators/fan` | Envia comando para o ventilador |
| POST | `/api/v1/actuators/exhaust` | Envia comando para o exaustor |
| GET | `/api/v1/alerts` | Lista alertas |
| POST | `/api/v1/alerts/resolve` | Resolve um alerta aberto |

Exemplo de criação de medição:

```bash
curl -X POST http://localhost:8080/api/v1/meansurements \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": 1,
    "temperature": 35,
    "humidity": 60,
    "luminosity": 800,
    "air_quality": 90
  }'
```

Quando uma métrica ultrapassa um limite configurado em `sensor_limits`, o `MeansurementService` cria automaticamente um registro em `alerts`.

## Status do desenvolvimento

### Concluído

- infraestrutura Docker;
- Nginx + PHP-FPM;
- conexão com PostgreSQL;
- migrations principais;
- API de medições;
- listagem de devices;
- Mosquitto;
- consumer MQTT;
- persistência de telemetria;
- atualização de `last_seen`;
- controle MQTT de fan e exhaust;
- persistência dos comandos;
- ACK de execução dos atuadores;
- limites ambientais;
- geração automática de alertas;
- listagem e resolução de alertas.

### Próximos passos

- consulta individual de device;
- cálculo automático de device online/offline;
- filtros por device e período no histórico;
- validações de faixa das métricas;
- endpoints de configuração de limites;
- padronização de erros;
- dashboard Web com Chart.js;
- integração física do ESP32;
- sensores reais;
- aplicação iOS.

## Execução local

Crie o arquivo `.env` a partir do exemplo:

```bash
cp .env.example .env
```

Preencha as variáveis do PostgreSQL e suba os serviços:

```bash
docker compose up -d --build
```

Serviços principais:

- Nginx / API: `http://localhost:8080`
- PostgreSQL: porta `5432`
- Mosquitto MQTT: porta `1883`
- MQTT consumer: executado como serviço do Docker Compose

Para acompanhar a telemetria:

```bash
docker compose logs -f mqtt_consumer
```

## Decisões de arquitetura

A primeira versão do EcoSense foi mantida propositalmente simples.

Redis e Kafka não fazem parte da V1. Eles só serão considerados se surgir uma necessidade concreta de cache, processamento distribuído ou alto volume de eventos.

A arquitetura atual é suficiente para o escopo:

```text
ESP32 + MQTT + PHP + PostgreSQL + Web + iOS
```

## Segurança

Credenciais locais ficam em `.env`, que não é versionado.

O arquivo `.env.example` documenta apenas as variáveis necessárias e pode ser mantido no repositório sem valores sensíveis.

## Licença

Este projeto é distribuído sob a licença MIT. Consulte o arquivo [LICENSE](LICENSE) para os termos completos.

---

**EcoSense — Monitorar. Entender. Agir.**
