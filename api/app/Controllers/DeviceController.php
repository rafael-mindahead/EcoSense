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
}