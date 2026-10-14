# EcoSense

EcoSense é um sistema ciber-físico de monitoramento ambiental desenvolvido com ESP32, MQTT, PHP, PostgreSQL, Docker e interfaces Web/iOS.

O projeto monitora variáveis ambientais, armazena medições, acompanha o estado dos dispositivos, avalia limites configurados, gera alertas e envia comandos para atuadores como ventiladores e exaustores.

> **Status atual:** infraestrutura, API REST, fluxo MQTT, PostgreSQL, telemetria do ESP32 físico, histórico filtrável, status dinâmico de devices, alertas, limites ambientais, controle de atuadores e testes de integração já estão implementados. Os sensores ainda são simulados no firmware.

## Objetivo

O EcoSense foi pensado para ambientes que precisam de acompanhamento contínuo, como estufas, laboratórios e espaços controlados.

A solução foi projetada para:

- monitorar temperatura e umidade;
- monitorar luminosidade;
- monitorar qualidade do ar;
- manter histórico de medições;
- acompanhar dispositivos online/offline;
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
  ├── MeansurementValidator
  ├── MeansurementRepository
  ├── DeviceRepository
  ├── SensorLimitRepository
  └── AlertService → AlertRepository
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
ActuatorService
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

O ciclo de comando possui confirmação por ACK:

```text
pending → sent → executed
```

## MCS — Maximum Create Service

O projeto segue o conceito **MCS (Maximum Create Service)**: concentrar regras de negócio na camada de serviços antes de finalizar Web e Mobile.

A ideia é manter as interfaces finas e reutilizar as mesmas regras independentemente da origem da requisição:

```text
REST ─────┐
          │
MQTT ─────┼──→ Service Layer → Repository → PostgreSQL
          │
iOS ──────┘
```

Responsabilidades:

```text
Controller → entrada e resposta HTTP
Service    → regras de negócio
Repository → persistência e consultas SQL
Worker     → adaptação MQTT
```

### Status MCS

- `MeansurementService` — concluído;
- `DeviceService` — concluído;
- `SensorLimitService` — concluído;
- `AlertService` — concluído;
- `ActuatorService` — concluído;
- `MqttService` — concluído.

**MCS V1: 100% concluído.**

## Tecnologias

### Embarcado

- ESP32
- C++
- Wi-Fi
- MQTT
- PlatformIO

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

### Front-end

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
│   │   ├── Controllers/
│   │   ├── Repositories/
│   │   ├── Services/
│   │   └── Validators/
│   ├── routes/
│   └── workers/
├── database/
├── docker/
├── esp32/
│   ├── include/
│   ├── src/
│   └── platformio.ini
├── tests/
├── web/
├── .env.example
├── docker-compose.yml
└── README.md
```

## ESP32

O firmware fica em `esp32/` e utiliza PlatformIO com framework Arduino.

Crie a configuração local a partir de:

```text
esp32/include/secrets.example.h
```

copiando para:

```text
esp32/include/secrets.h
```

O arquivo `secrets.h` é local e não deve ser versionado. Além das credenciais Wi-Fi, ele também define `MQTT_HOST`, evitando manter o IP do broker fixo no código-fonte.

O firmware atual publica telemetria simulada em:

```text
ecosense/device/1/telemetry
```

Exemplo:

```json
{
  "temperature": 27.1,
  "humidity": 63.2,
  "luminosity": 820,
  "air_quality": 94
}
```

Quando os sensores físicos forem conectados, a geração simulada será substituída pelas leituras reais sem alterar o restante do pipeline.

## Banco de dados

Entidades implementadas:

- `devices`
- `meansurements`
- `actuator_commands`
- `sensor_limits`
- `alerts`

O projeto mantém a grafia `meansurements` na API e no banco por compatibilidade com a implementação atual.

## MQTT

Tópicos principais:

```text
ecosense/device/+/telemetry
ecosense/device/+/commands/ack
ecosense/device/{id}/commands/fan
ecosense/device/{id}/commands/exhaust
```

O consumer processa cada telemetria válida uma única vez, reutilizando o mesmo `MeansurementService` usado pela API REST.

## API REST

| Método | Endpoint | Descrição |
|---|---|---|
| GET | `/api/health` | Estado da API |
| GET | `/api/database/health` | Estado do PostgreSQL |
| GET | `/api/v1/meansurements/latest` | Última medição |
| GET | `/api/v1/meansurements/latest?device_id=1` | Última medição de um device |
| GET | `/api/v1/meansurements?limit=50` | Histórico |
| GET | `/api/v1/meansurements?device_id=1&from=YYYY-MM-DD&to=YYYY-MM-DD&limit=50` | Histórico filtrado |
| POST | `/api/v1/meansurements` | Registra medição e processa alertas |
| GET | `/api/v1/devices` | Lista devices e status dinâmico |
| GET | `/api/v1/devices/show?id=1` | Consulta um device |
| POST | `/api/v1/actuators/fan` | Comando do ventilador |
| POST | `/api/v1/actuators/exhaust` | Comando do exaustor |
| GET | `/api/v1/alerts` | Lista alertas |
| POST | `/api/v1/alerts/resolve` | Resolve alerta |
| GET | `/api/v1/sensor-limits` | Lista limites |
| POST | `/api/v1/sensor-limits` | Cria limite |
| POST | `/api/v1/sensor-limits/update` | Atualiza limite |

### Status dinâmico dos devices

A API calcula o estado a partir de `last_seen`:

```text
última comunicação <= 30 segundos → online
última comunicação > 30 segundos  → offline
sem last_seen                      → offline
```

## Testes

A suíte cobre atualmente:

- health da API;
- criação e validação de medições;
- latest e histórico filtrado;
- consulta e validação de devices;
- validações de sensor limits;
- listagem e validação de alertas;
- comandos de atuadores e transição `sent → executed` por ACK;
- proteção contra ACK duplicado sem derrubar o consumer;
- fluxo MQTT → consumer → PostgreSQL;
- proteção contra gravação MQTT duplicada;
- rejeição de telemetria MQTT inválida sem derrubar o consumer.

Execute na raiz:

```bash
./tests/run.sh
```

## Execução local

Crie o arquivo de ambiente:

```bash
cp .env.example .env
```

Suba os serviços:

```bash
docker compose up -d --build
```

Confira:

```bash
docker compose ps
```

Acompanhe a telemetria:

```bash
docker compose logs -f mqtt_consumer
```

## Status do desenvolvimento

### Concluído

- infraestrutura Docker;
- PostgreSQL e migrations;
- Mosquitto;
- consumer MQTT;
- persistência de telemetria;
- API REST;
- camada MCS V1 concluída;
- `MeansurementService`, `DeviceService`, `SensorLimitService`, `AlertService`, `ActuatorService` e `MqttService` centralizados;
- validação compartilhada entre REST e MQTT;
- histórico por device e período;
- status online/offline por `last_seen`;
- limites ambientais e geração de alertas;
- controle de atuadores e ACK;
- suíte de integração para API e MQTT;
- dashboard Web inicial;
- firmware ESP32 com Wi-Fi e MQTT;
- telemetria simulada publicada pelo ESP32 físico.

### Próximos passos

- servir o dashboard Web pela infraestrutura Docker/Nginx;
- conectar o histórico do dashboard ao Chart.js;
- implementar assinatura de comandos e ACK real no firmware ESP32;
- integrar sensores físicos ao ESP32;
- finalizar páginas Web de histórico, alertas, dispositivos e configurações;
- iniciar aplicação iOS consumindo os mesmos contratos da API.

## Reforma para ESP32 servidor

A branch `reformar/requisito` transforma o ESP32 no servidor autônomo exigido pelo TDE. Nessa arquitetura, Web, API local, performance, histórico, logs, configuração, OTA e controle dos atuadores funcionam no próprio dispositivo. MQTT/PHP/PostgreSQL passam a ser uma integração externa opcional, e não uma dependência para o funcionamento local.

A matriz detalhada de aderência está em `Docs/ESP_SERVER_REQUIREMENTS.md`.

## Segurança

Credenciais locais ficam fora do versionamento.

Arquivos como `.env` e `esp32/include/secrets.h` são ignorados. Apenas arquivos de exemplo sem valores sensíveis devem ser versionados.

## Licença

Este projeto é distribuído sob a licença MIT. Consulte o arquivo [LICENSE](LICENSE).

---

**EcoSense — Monitorar. Entender. Agir.**
