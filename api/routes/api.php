<?php

use App\Core\Response;

$router->get('/api/health', function () {

    Response::json([
        'status' => 'online',
        'service' => 'EcoSense API',
        'version' => '1.0.0'
    ]);

});