<?php

use App\Core\Response;
use  App\Config\Database;
use App\Controllers\MeansurementController;
use App\Repositories\MeansurementRepository;


$router->get('/api/health', function () {

    Response::json([
        'status' => 'online',
        'service' => 'EcoSense API',
        'version' => '1.0.0'
    ]);
});
$router->get('/api/database/health',function(){
    try {
        $connection = Database::connect();

        Response::json([
            'status' => 'online',
            'database' => 'PostgreSQL'
        ]);
    } catch (\Throwable $error){
        Response::json([
            'status' => 'error',
            'message' => $error->getMessage()
        ], 500);
    }
});
$router->get('/api/v1/meansurements/latest', function () {

    $connection = Database::connect();

    $repository = new MeansurementRepository(
        $connection
    );

    $controller = new MeansurementController(
        $repository
    );

    $controller->latest();
});
