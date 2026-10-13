<?php

namespace App\Controllers;

use App\Core\Response;
use App\Services\AlertService;

class AlertController
{
    public function __construct(
        private AlertService $service
    ) {
    }

    public function index(): void
    {
        try {

            Response::json(
                $this->service->list()
            );

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' =>
                    $error->getMessage()
            ], 500);
        }
    }

    public function resolve(): void
    {
        try {

            $body = json_decode(
                file_get_contents(
                    'php://input'
                ),
                true
            );

            if (!is_array($body)) {

                Response::json([
                    'error' =>
                        'JSON invalido'
                ], 400);

                return;
            }

            if (!array_key_exists(
                'alert_id',
                $body
            )) {
                throw new \InvalidArgumentException(
                    'alert_id is required'
                );
            }

            $alert =
                $this->service
                    ->resolve(
                        $body['alert_id']
                    );

            Response::json([
                'message' =>
                    'Alert resolved',
                'data' => $alert
            ]);

        } catch (
            \InvalidArgumentException $error
        ) {

            Response::json([
                'error' =>
                    $error->getMessage()
            ], 422);

        } catch (
            \RuntimeException $error
        ) {

            Response::json([
                'error' =>
                    $error->getMessage()
            ], 404);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' =>
                    $error->getMessage()
            ], 500);
        }
    }
}