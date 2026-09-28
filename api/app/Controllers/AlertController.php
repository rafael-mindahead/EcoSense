<?php

namespace App\Controllers;

use App\Core\Response;
use App\Repositories\AlertRepository;

class AlertController
{
    public function __construct(
        private AlertRepository $repository
    ) {
    }

    public function index(): void
    {
        try {

            $alerts =
                $this->repository->findAll();

            Response::json([
                'count' => count($alerts),
                'data' => $alerts
            ]);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' => $error->getMessage()
            ], 500);
        }
    }

    public function resolve(): void
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

            if (!isset($body['alert_id'])) {

                Response::json([
                    'error' => 'alert_id is required'
                ], 422);

                return;
            }

            $alertId =
                (int) $body['alert_id'];

            if ($alertId <= 0) {

                Response::json([
                    'error' => 'alert_id invalido'
                ], 422);

                return;
            }

            $alert =
                $this->repository
                    ->resolve($alertId);

            if (!$alert) {

                Response::json([
                    'error' =>
                        'Alert not found or already resolved'
                ], 404);

                return;
            }

            Response::json([
                'message' => 'Alert resolved',
                'data' => $alert
            ]);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' => $error->getMessage()
            ], 500);
        }
    }
}