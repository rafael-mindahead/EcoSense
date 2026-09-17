<?php

namespace App\Controllers;

use App\Core\Response;
use App\Repositories\MeansurementRepository;
use App\Repositories\DeviceRepository;

class MeansurementController
{
    public function __construct(
        private MeansurementRepository $repository,
        private DeviceRepository $deviceRepository
    ) {
    }

    public function latest(): void
    {
        try {

            $meansurement = $this->repository->findLatest();

            if (!$meansurement) {

                Response::json([
                    'error' => 'No meansurements found'
                ], 404);

                return;
            }

            Response::json($meansurement);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' => $error->getMessage()
            ], 500);
        }
    }

    public function index(): void
    {
        try {

            $limit = isset($_GET['limit'])
                ? (int) $_GET['limit']
                : 50;

            $limit = max(1, min($limit, 100));

            $meansurements =
                $this->repository->findAll($limit);

            Response::json([
                'count' => count($meansurements),
                'limit' => $limit,
                'data' => $meansurements
            ]);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' => $error->getMessage()
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

            $requiredFields = [
                'device_id',
                'temperature',
                'humidity',
                'luminosity',
                'air_quality'
            ];

            foreach ($requiredFields as $field) {

                if (!array_key_exists($field, $body)) {

                    Response::json([
                        'error' => "Field {$field} is required"
                    ], 422);

                    return;
                }
            }

            $meansurement =
                $this->repository->create($body);

            $device = $this->deviceRepository->markOnline(
                (int) $body['device_id']
            );

            Response::json([
                'message' => 'Meansurement criado',
                'data' => $meansurement,
                'device' => $device
            ], 201);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' => $error->getMessage()
            ], 500);
        }
    }
}