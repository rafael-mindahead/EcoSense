<?php

use App\Core\Response;
use App\Config\Database;

use App\Controllers\MeansurementController;
use App\Repositories\MeansurementRepository;

use App\Controllers\DeviceController;
use App\Repositories\DeviceRepository;

use App\Controllers\ActuatorController;
use App\Repositories\ActuatorCommandRepository;
use App\Services\MqttService;

use App\Repositories\SensorLimitRepository;
use App\Repositories\AlertRepository;
use App\Services\MeansurementService;

use App\Controllers\AlertController;


$router->get('/api/health', function () {

    Response::json([
        'status' => 'online',
        'service' => 'EcoSense API',
        'version' => '1.0.0'
    ]);
});


$router->get('/api/database/health', function () {

    try {

        $connection = Database::connect();

        Response::json([
            'status' => 'online',
            'database' => 'PostgreSQL'
        ]);

    } catch (\Throwable $error) {

        Response::json([
            'status' => 'error',
            'message' => $error->getMessage()
        ], 500);
    }
});


$router->get('/api/v1/meansurements/latest', function () {

    $connection = Database::connect();
    $repository =
    new MeansurementRepository($connection);

    $deviceRepository =
        new DeviceRepository($connection);

    $sensorLimitRepository =
        new SensorLimitRepository($connection);

    $alertRepository =
        new AlertRepository($connection);

    $service =
        new MeansurementService(
            $repository,
            $deviceRepository,
            $sensorLimitRepository,
            $alertRepository
        );

    $controller =
        new MeansurementController(
            $repository,
            $service
        );
});


$router->get('/api/v1/meansurements', function () {

    $connection = Database::connect();

    $repository =
    new MeansurementRepository($connection);

    $deviceRepository =
        new DeviceRepository($connection);

    $sensorLimitRepository =
        new SensorLimitRepository($connection);

    $alertRepository =
        new AlertRepository($connection);

    $service =
        new MeansurementService(
            $repository,
            $deviceRepository,
            $sensorLimitRepository,
            $alertRepository
        );

    $controller =
        new MeansurementController(
            $repository,
            $service
        );
});


$router->post('/api/v1/meansurements', function () {

    $connection = Database::connect();
    $repository =
        new MeansurementRepository($connection);

    $deviceRepository =
        new DeviceRepository($connection);

    $sensorLimitRepository =
        new SensorLimitRepository($connection);

    $alertRepository =
        new AlertRepository($connection);

    $service =
        new MeansurementService(
            $repository,
            $deviceRepository,
            $sensorLimitRepository,
            $alertRepository
        );

    $controller =
        new MeansurementController(
            $repository,
            $service
        );
    $controller->store();
});


$router->get('/api/v1/devices', function () {

    $connection = Database::connect();

    $repository =
        new DeviceRepository($connection);

    $controller =
        new DeviceController($repository);

    $controller->index();
});

$router->post('/api/v1/actuators/fan', function () {

    $connection = Database::connect();

    $repository =
        new ActuatorCommandRepository(
            $connection
        );

    $mqtt =
        new MqttService();

    $controller =
        new ActuatorController(
            $repository,
            $mqtt
        );

    $controller->control('fan');
});

$router->post('/api/v1/actuators/exhaust', function () {

    $connection = Database::connect();

    $repository =
        new ActuatorCommandRepository(
            $connection
        );

    $mqtt =
        new MqttService();

    $controller =
        new ActuatorController(
            $repository,
            $mqtt
        );

    $controller->control('exhaust');
});
$router->get('/api/v1/alerts', function () {

    $connection = Database::connect();

    $repository =
        new AlertRepository($connection);

    $controller =
        new AlertController($repository);

    $controller->index();
});


$router->post('/api/v1/alerts/resolve', function () {

    $connection = Database::connect();

    $repository =
        new AlertRepository($connection);

    $controller =
        new AlertController($repository);

    $controller->resolve();
});