<?php

namespace App\Controllers;

use App\Core\Response;
use App\Repositories\MeansurementRepository;
use App\Services\MeansurementService;
use App\Validators\MeansurementValidator;

class MeansurementController
{
    public function __construct(
        private MeansurementService $service,
    ) {
    }

    public function latest(): void
    {
        try {

            $meansurement =
                $this->service->latest(
                    $_GET['device_id']
                    ?? null
                );

            if (!$meansurement) {

                Response::json([
                    'error' =>
                        'No meansurements found'
                ], 404);

                return;
            }

            Response::json(
                $meansurement
            );

        } catch (\InvalidArgumentException $error) {

            Response::json([
                'error' =>
                    $error->getMessage()
            ], 422);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' =>
                    $error->getMessage()
            ], 500);
        }
    }
    public function index(): void
    {
        try {

            $result =
                $this->service
                    ->history($_GET);

            Response::json(
                $result
            );

        } catch (\InvalidArgumentException $error) {

            Response::json([
                'error' =>
                    $error->getMessage()
            ], 422);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' =>
                    $error->getMessage()
            ], 500);
        }
    }
    public function store(): void
    {
        try {

            $body = json_decode(
                file_get_contents('php://input'),
                true
            );

            if (!is_array($body)) {

                Response::json([
                    'error' => 'JSON invalido'
                ], 400);

                return;
            }

            $result = $this->service->create($body);

            Response::json([
                'message' => 'Meansurement criado',
                'data' => $result['meansurement'],
                'alerts' => $result['alerts']
            ], 201);

        } catch (\InvalidArgumentException $error) {

            Response::json([
                'error' => 'validation failed',
                
                'error' =>
                    $error->getMessage()
            ], 422);
        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' => $error->getMessage()
            ], 500);
        } 
    }
}
