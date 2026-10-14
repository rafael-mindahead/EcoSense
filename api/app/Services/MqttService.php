<?php

namespace App\Services;

use JsonException;
use PhpMqtt\Client\MqttClient;

class MqttService
{
    private string $host;
    private int $port;

    public function __construct()
    {
        $this->host =
            getenv('MQTT_HOST')
            ?: 'mosquitto';

        $this->port =
            (int) (
                getenv('MQTT_PORT')
                ?: 1883
            );
    }

    public function publish(
        string $topic,
        array $payload
    ): void {

        $topic = trim($topic);

        if ($topic === '') {
            throw new \InvalidArgumentException(
                'MQTT topic cannot be empty'
            );
        }

        try {

            $message = json_encode(
                $payload,
                JSON_THROW_ON_ERROR
            );

        } catch (JsonException $error) {

            throw new \RuntimeException(
                'Failed to encode MQTT payload',
                0,
                $error
            );
        }

        $clientId =
            'ecosense-api-'
            . uniqid();

        $mqtt =
            new MqttClient(
                $this->host,
                $this->port,
                $clientId
            );

        $connected = false;

        try {

            $mqtt->connect();

            $connected = true;

            $mqtt->publish(
                $topic,
                $message,
                0
            );

        } finally {

            if ($connected) {
                $mqtt->disconnect();
            }
        }
    }
}