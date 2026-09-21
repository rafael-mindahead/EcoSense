<?php

require_once __DIR__ . '/../vendor/autoload.php';

use App\Config\Database;
use App\Repositories\MeansurementRepository;
use App\Repositories\DeviceRepository;
use App\Repositories\ActuatorCommandRepository;
use PhpMqtt\Client\MqttClient;


// Configuração MQTT
$host = getenv('MQTT_HOST') ?: 'mosquitto';

$port = (int) (
    getenv('MQTT_PORT') ?: 1883
);

$clientId =
    'ecosense-consumer-' . uniqid();


// Banco
$connection = Database::connect();

$meansurementRepository =
    new MeansurementRepository($connection);

$deviceRepository =
    new DeviceRepository($connection);

$actuatorRepository =
    new ActuatorCommandRepository($connection);


// Cliente MQTT
$mqtt = new MqttClient(
    $host,
    $port,
    $clientId
);

$mqtt->connect();


// ===============================
// TELEMETRIA
// ===============================

$telemetryTopic =
    'ecosense/device/+/telemetry';

$mqtt->subscribe(
    $telemetryTopic,

    function (
        string $topic,
        string $message,
        bool $retained,
        array $matchedWildcards
    ) use (
        $meansurementRepository,
        $deviceRepository
    ) {

        echo "Mensagem recebida: {$message}"
            . PHP_EOL;

        $data = json_decode(
            $message,
            true
        );


        if (!is_array($data)) {

            echo "JSON invalido."
                . PHP_EOL;

            return;
        }


        $topicParts = explode(
            '/',
            $topic
        );

        $deviceId =
            (int) ($topicParts[2] ?? 0);


        if ($deviceId <= 0) {

            echo "device_id invalido."
                . PHP_EOL;

            return;
        }


        $requiredFields = [
            'temperature',
            'humidity',
            'luminosity',
            'air_quality'
        ];


        foreach ($requiredFields as $field) {

            if (!array_key_exists(
                $field,
                $data
            )) {

                echo
                    "Campo {$field} ausente."
                    . PHP_EOL;

                return;
            }
        }


        $data['device_id'] =
            $deviceId;


        $meansurement =
            $meansurementRepository
                ->create($data);


        $deviceRepository
            ->markOnline($deviceId);


        echo
            "Meansurement salva. ID: "
            . $meansurement['id']
            . PHP_EOL;
    },

    0
);


// ===============================
// ACK DOS ATUADORES
// ===============================

$ackTopic =
    'ecosense/device/+/commands/ack';

$mqtt->subscribe(
    $ackTopic,

    function (
        string $topic,
        string $message,
        bool $retained,
        array $matchedWildcards
    ) use (
        $actuatorRepository
    ) {

        echo "ACK recebido: {$message}"
            . PHP_EOL;


        $data = json_decode(
            $message,
            true
        );


        if (!is_array($data)) {

            echo "ACK JSON invalido."
                . PHP_EOL;

            return;
        }


        if (!isset($data['command_id'])) {

            echo "command_id ausente."
                . PHP_EOL;

            return;
        }


        $commandId =
            (int) $data['command_id'];


        $command =
            $actuatorRepository
                ->markExecuted(
                    $commandId
                );


        if (!$command) {

            echo
                "Comando nao encontrado."
                . PHP_EOL;

            return;
        }


        echo
            "Comando {$commandId} executado."
            . PHP_EOL;
    },

    0
);


// ===============================
// LOOP
// ===============================

echo "EcoSense MQTT Consumer iniciado."
    . PHP_EOL;

echo "Escutando telemetria: "
    . $telemetryTopic
    . PHP_EOL;

echo "Escutando ACK: "
    . $ackTopic
    . PHP_EOL;


$mqtt->loop(true);