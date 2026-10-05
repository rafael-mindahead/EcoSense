# EcoSense

EcoSense é um sistema ciber-físico de monitoramento ambiental desenvolvido com ESP32, MQTT, PHP, PostgreSQL, Docker e interfaces Web/iOS.

O projeto monitora variáveis ambientais, armazena medições, avalia limites configurados, gera alertas e envia comandos para atuadores como ventiladores e exaustores.

> Status: em desenvolvimento. A infraestrutura, a API REST, o fluxo MQTT, alertas, controle de atuadores e a integração inicial com o ESP32 físico já estão implementados. O ESP32 publica telemetria simulada por MQTT; a próxima etapa física é substituir a simulação pelos sensores reais.

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
├── database/
├── docker/
├── esp32/
│   ├── include/
│   ├── src/
│   └── platformio.ini
├── tests/
├── .env.example
├── docker-compose.yml
└── README.md
```

## ESP32

O firmware fica em `esp32/` e utiliza PlatformIO com o framework Arduino.

A configuração local de Wi-Fi deve ser criada a partir de:

```text
esp32/include/secrets.example.h
```

Copie para:

```text
esp32/include/secrets.h
```

e preencha apenas localmente. Esse arquivo é ignorado pelo Git.

O firmware atual publica telemetria simulada no tópico:

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

## Banco de dados

Entidades implementadas:

- `devices`
- `meansurements`
- `actuator_commands`
- `sensor_limits`
- `alerts`

O projeto mantém a grafia `meansurements` na API e no banco por compatibilidade com a implementação atual.

## MQTT

Tópicos utilizados:

```text
ecosense/device/+/telemetry
ecosense/device/+/commands/ack
ecosense/device/{id}/commands/fan
ecosense/device/{id}/commands/exhaust
```

## API REST

Endpoints principais:

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
| GET | `/api/v1/sensor-limits` | Lista limites dos sensores |
| POST | `/api/v1/sensor-limits` | Cria limite de sensor |
| POST | `/api/v1/sensor-limits/update` | Atualiza limite de sensor |

## Testes

A pasta `tests/` contém testes de integração da API e do fluxo MQTT.

Execute:

```bash
./tests/run.sh
```

## Execução local

Crie o arquivo `.env`:

```bash
cp .env.example .env
```

Suba os serviços:

```bash
docker compose up -d --build
```

Para acompanhar a telemetria:

```bash
docker compose logs -f mqtt_consumer
```

## Status do desenvolvimento

### Concluído

- infraestrutura Docker;
- API REST;
- PostgreSQL e migrations;
- Mosquitto;
- consumer MQTT;
- persistência de telemetria;
- limites ambientais e alertas;
- controle de atuadores e ACK;
- validação de medições;
- testes de integração iniciais;
- dashboard Web inicial;
- firmware ESP32 com Wi-Fi e MQTT;
- publicação de telemetria simulada a partir do ESP32 físico.

### Próximos passos

- integrar sensores reais ao ESP32;
- consulta individual e status automático de devices;
- filtros de histórico;
- expandir testes automatizados;
- finalizar dashboard Web;
- aplicação iOS.

## Segurança

Credenciais locais ficam fora do versionamento.

Arquivos como `.env` e `esp32/include/secrets.h` são ignorados. Apenas arquivos de exemplo sem valores sensíveis são versionados.

## Licença

Este projeto é distribuído sob a licença MIT. Consulte o arquivo [LICENSE](LICENSE).

---

**EcoSense — Monitorar. Entender. Agir.**
