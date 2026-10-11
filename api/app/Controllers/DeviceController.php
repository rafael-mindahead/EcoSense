<?php

namespace App\Controllers;

use App\Core\Response;
use App\Services\DeviceService;

class DeviceController
{
    public function __construct(
        private DeviceService $service
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

    public function show(): void
    {
        try {

            if (!isset($_GET['id'])) {

                Response::json([
                    'error' =>
                        'id is required'
                ], 422);

                return;
            }

            $device =
                $this->service->getById(
                    $_GET['id']
                );

            Response::json([
                'data' => $device
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