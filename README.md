🌱 EcoSense

Sistema ciber-físico de monitoramento ambiental inteligente desenvolvido com ESP32, API REST em PHP, PostgreSQL, MQTT e interfaces Web/iOS.

O objetivo do EcoSense é monitorar variáveis ambientais, armazenar medições, gerar alertas e permitir o acionamento de dispositivos físicos, como ventiladores e exaustores.

Status atual: 🚧 Em desenvolvimento — estrutura inicial da API e infraestrutura Docker.

🎯 Objetivo

O EcoSense foi pensado para monitorar ambientes como estufas, laboratórios e outros espaços que precisam de acompanhamento constante de condições ambientais.

O sistema deverá ser capaz de:

Monitorar temperatura e umidade;

Monitorar luminosidade;

Monitorar qualidade do ar;

Armazenar histórico de medições;

Exibir dados em um dashboard web;

Gerar gráficos com Chart.js;

Permitir acesso por aplicativo iOS;

Enviar e receber mensagens por MQTT;

Acionar ventiladores e exaustores por meio de módulos de acionamento.

🏗️ Arquitetura planejada

Sensores ↓ ESP32 / C++ ↓ Wi-Fi / MQTT ↓ Mosquitto ↓ PHP API REST ↓ PostgreSQL ↑ ├───────────────┐ │ │ Dashboard Web App iOS HTML/CSS/JS Swift/SwiftUI Chart.js

Fluxo de acionamento

Web / iOS ↓ PHP API REST ↓ MQTT ↓ ESP32 ↓ Módulo Relé ↓ Ventilador / Exaustor

🧰 Tecnologias

Embarcado

ESP32

C++

Backend

PHP 8.3

API REST

Composer

Banco de dados

PostgreSQL

Comunicação IoT

MQTT

Mosquitto

Front-end Web

HTML

CSS

JavaScript

Chart.js

Mobile

Swift

SwiftUI

iOS

Infraestrutura

Docker

Docker Compose

Nginx

PHP-FPM

📁 Estrutura inicial do projeto

EcoSense/ │ ├── api/ │ ├── app/ │ │ └── Core/ │ │ ├── Response.php │ │ └── Router.php │ │ │ ├── public/ │ │ └── index.php │ │ │ └── routes/ │ └── api.php │ ├── docker/ │ ├── nginx/ │ │ └── default.conf │ │ │ └── php/ │ └── Dockerfile │ ├── .env ├── .env.example ├── composer.json ├── docker-compose.yml └── README.md

🐳 Infraestrutura com Docker

A aplicação será executada com Docker Compose.

Nginx

Responsável por receber requisições HTTP e encaminhá-las ao PHP-FPM.

PHP

Responsável pela API REST e pelas regras de negócio.

PostgreSQL

Responsável pelo armazenamento das medições, dispositivos, alertas e histórico.

Mosquitto

Será adicionado posteriormente para realizar a comunicação MQTT com o ESP32.

🔌 API REST

A API será o ponto central de comunicação entre os clientes do sistema.

Clientes previstos:

Dashboard Web;

Aplicativo iOS;

Futuras integrações.

Endpoint inicial

GET /api/health

Resposta esperada:

{ "status": "online", "service": "EcoSense API", "version": "1.0.0" }

📊 Endpoints planejados

Medições

GET /api/v1/measurements

GET /api/v1/measurements/latest

Dispositivos

GET /api/v1/devices/{id}

Atuadores

POST /api/v1/actuators/fan

POST /api/v1/actuators/exhaust

Alertas

GET /api/v1/alerts

🗃️ Banco de dados planejado

Principais entidades:

devices measurements actuator_commands alerts

Relacionamento principal:

devices │ │ 1 │ │ N ▼ measurements

Um dispositivo ESP32 poderá registrar diversas medições ao longo do tempo.

📡 MQTT

A comunicação com o ESP32 será feita por MQTT.

Exemplos de tópicos planejados:

ecosense/device/1/telemetry ecosense/device/1/status ecosense/device/1/commands/fan ecosense/device/1/commands/exhaust

Exemplo de telemetria:

{ "temperature": 27.1, "humidity": 63.2, "luminosity": 820, "air_quality": 94 }

🚧 Estado atual do desenvolvimento

Até o momento, a implementação foi iniciada pela infraestrutura e pelo núcleo básico da API.

Concluído

Definição da arquitetura inicial;

Estrutura de pastas;

Docker Compose inicial;

Dockerfile do PHP;

Configuração inicial do Nginx;

Composer e autoload PSR-4;

Classe Response.php.

Próximos passos

Criar Router.php;

Criar arquivo de rotas api.php;

Criar public/index.php;

Configurar .env;

Subir os containers;

Testar GET /api/health;

Conectar PHP ao PostgreSQL;

Criar as primeiras tabelas;

Implementar endpoints de medições;

Adicionar Mosquitto;

Integrar ESP32 por MQTT;

Criar dashboard Web;

Integrar aplicativo iOS.

🧠 Decisões de arquitetura

A primeira versão do EcoSense será mantida propositalmente simples.

Neste momento, Redis e Kafka não fazem parte da arquitetura.

Eles só deverão ser adicionados caso surja uma necessidade concreta de:

cache;

grande volume de eventos;

processamento assíncrono;

mensageria distribuída;

escalabilidade adicional.

Para a primeira versão:

ESP32 + MQTT + PHP + PostgreSQL + Web + iOS

é suficiente para atender ao escopo atual.

🔐 Segurança

Arquivos contendo credenciais não deverão ser enviados ao repositório.

O arquivo:

.env

deverá estar presente no .gitignore.

Um arquivo:

.env.example

poderá ser disponibilizado sem valores sensíveis para documentar as variáveis necessárias.

🌿 EcoSense

Monitorar. Entender. Agir.

Projeto de monitoramento ambiental utilizando sistemas embarcados, IoT, desenvolvimento Web, APIs e aplicações móveis.
