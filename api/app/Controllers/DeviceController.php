<?php

namespace App\Controllers;

use App\Core\Response;
use App\Repositories\DeviceRepository;

class DeviceController
{
    public function __construct(
        private DeviceRepository $repository
    ) {
    }

    public function index(): void
    {
        try {

            $devices = $this->repository->findAll();

            Response::json([
                'count' => count($devices),
                'data' => $devices
            ]);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' => $error->getMessage()
            ], 500);
        }
    }
    public function show(): void
    {
        try {

            if (!isset($_GET['id'])) {

                Response::json([
                    'error' => 'id is required'
                ], 422);

                return;
            }

            $deviceId =
                (int) $_GET['id'];

            if ($deviceId <= 0) {

                Response::json([
                    'error' => 'invalid device id'
                ], 422);

                return;
            }

            $device =
                $this->repository
                    ->findById($deviceId);

            if (!$device) {

                Response::json([
                    'error' => 'device not found'
                ], 404);

                return;
            }

            Response::json([
                'data' => $device
            ]);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' =>
                    $error->getMessage()
            ], 500);
        }
    }
}