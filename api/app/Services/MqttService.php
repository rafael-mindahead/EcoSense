<?php

namespace App\Services;

use PhpMqtt\Client\MqttClient;

class MqttService
{
    private string $host;
    private int $port;

    public function __construct()
    {
        $this->host =
            getenv('MQTT_HOST') ?: 'mosquitto';

        $this->port =
            (int) (getenv('MQTT_PORT') ?: 1883);
    }

    public function publish(
        string $topic,
        array $payload
    ): void {

        $clientId =
            'ecosense-api-' . uniqid();

        $mqtt = new MqttClient(
            $this->host,
            $this->port,
            $clientId
        );

        $mqtt->connect();

        $mqtt->publish(
            $topic,
            json_encode($payload),
            0
        );

        $mqtt->disconnect();
    }
}