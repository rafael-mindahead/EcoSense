<?php

require_once __DIR__ . '/../vendor/autoload.php';

use App\Config\Database;
use App\Repositories\MeansurementRepository;
use App\Repositories\DeviceRepository;
use PhpMqtt\Client\MqttClient;


$host = getenv('MQTT_HOST') ?: 'mosquitto';

$port = (int) (getenv('MQTT_PORT') ?: 1883);

$clienteId = 'ecosense-telemetry-consumer-';

$connection = Database::connect();

$meansurementRepository = new MeansurementRepository($connection);

$deviceRepository = new DeviceRepository($connection);

$mqtt = new MqttClient(
    $host,
    $port,
    $clienteId
);
$mqtt->connect();

$topic = 'ecosense/device/1/telemetry';

$mqtt->subscribe(
    $topic,
    function(
        string $topic,
        string $message,
        bool $retained,
        array $matchedWildcards // lista de partes do topico que foram substituidas por coringas
    ) use (
        $meansurementRepository,
        $deviceRepository
    ){
        echo "mensagem recebida: {$message}" . PHP_EOL;

        $data = json_decode(
            $message,
            true
        );
        if (!is_array($data)) {
            echo "json invalido {$message}" . PHP_EOL;
            return;
        }
        $topicParts = explode('/', $topic);
        $deviceId =(int) ($topicParts[2]?? 0 );
        if ($deviceId <= 0){
            echo "device_id invalido {$message}" . PHP_EOL;
            return;
        }
        $requireFields = [
            'temperature',
            'humidity',
            'luminosity',
            'air_quality'
        ];
        foreach ($requireFields as $field) {
            if (!array_key_exists($field, $data)) {
                echo "campo {$field} ausente no json {$message}" . PHP_EOL;
                return;
            }
        }
        $data['device_id'] = $deviceId;

        $meansurement = $meansurementRepository->create($data);

        $deviceRepository->markOnline($deviceId);

        echo "Meansurement salva ID" . $meansurement['id'] . PHP_EOL;
    },
    0
);
echo "EcoSense MQTT Consumer iniciado" . PHP_EOL;

echo "escutando... {$topic}" . PHP_EOL;

$mqtt->loop(true);