<?php

namespace App\Controllers;

use App\Core\Response;
use App\Repositories\SensorLimitRepository;

class SensorLimitController
{
    private const ALLOWED_METRICS = [
        'temperature',
        'humidity',
        'luminosity',
        'air_quality'
    ];

    public function __construct(
        private SensorLimitRepository $repository
    ) {
    }

    public function index(): void
    {
        try {

            $deviceId = null;

            if (isset($_GET['device_id'])) {

                $deviceId =
                    (int) $_GET['device_id'];

                if ($deviceId <= 0) {

                    Response::json([
                        'error' =>
                            'device_id invalido'
                    ], 422);

                    return;
                }
            }

            $limits =
                $this->repository
                    ->findAll($deviceId);

            Response::json([
                'count' => count($limits),
                'data' => $limits
            ]);

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

            $body = $this->getBody();

            if ($body === null) {
                return;
            }

            if (!$this->validate($body)) {
                return;
            }

            $limit =
                $this->repository->create(
                    (int) $body['device_id'],
                    $body['metric'],
                    $this->toFloatOrNull(
                        $body['min_value'] ?? null
                    ),
                    $this->toFloatOrNull(
                        $body['max_value'] ?? null
                    ),
                    $body['enabled'] ?? true
                );

            if (!$limit) {

                Response::json([
                    'error' =>
                        'Sensor limit already exists'
                ], 409);

                return;
            }

            Response::json([
                'message' =>
                    'Sensor limit created',
                'data' => $limit
            ], 201);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' =>
                    $error->getMessage()
            ], 500);
        }
    }

    public function update(): void
    {
        try {

            $body = $this->getBody();

            if ($body === null) {
                return;
            }

            if (!$this->validate($body)) {
                return;
            }

            $limit =
                $this->repository->update(
                    (int) $body['device_id'],
                    $body['metric'],
                    $this->toFloatOrNull(
                        $body['min_value'] ?? null
                    ),
                    $this->toFloatOrNull(
                        $body['max_value'] ?? null
                    ),
                    $body['enabled'] ?? true
                );

            if (!$limit) {

                Response::json([
                    'error' =>
                        'Sensor limit not found'
                ], 404);

                return;
            }

            Response::json([
                'message' =>
                    'Sensor limit updated',
                'data' => $limit
            ]);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' =>
                    $error->getMessage()
            ], 500);
        }
    }

    private function getBody(): ?array
    {
        $body = json_decode(
            file_get_contents('php://input'),
            true
        );

        if (!is_array($body)) {

            Response::json([
                'error' => 'JSON invalido'
            ], 400);

            return null;
        }

        return $body;
    }

    private function validate(
        array $body
    ): bool {

        if (!isset(
            $body['device_id'],
            $body['metric']
        )) {

            Response::json([
                'error' =>
                    'device_id and metric are required'
            ], 422);

            return false;
        }

        if ((int) $body['device_id'] <= 0) {

            Response::json([
                'error' =>
                    'device_id invalido'
            ], 422);

            return false;
        }

        if (!in_array(
            $body['metric'],
            self::ALLOWED_METRICS,
            true
        )) {

            Response::json([
                'error' =>
                    'metric invalida'
            ], 422);

            return false;
        }

        $minValue =
            $body['min_value'] ?? null;

        $maxValue =
            $body['max_value'] ?? null;

        if (
            $minValue === null &&
            $maxValue === null
        ) {

            Response::json([
                'error' =>
                    'min_value or max_value is required'
            ], 422);

            return false;
        }

        if (
            $minValue !== null &&
            !is_numeric($minValue)
        ) {

            Response::json([
                'error' =>
                    'min_value must be numeric'
            ], 422);

            return false;
        }

        if (
            $maxValue !== null &&
            !is_numeric($maxValue)
        ) {

            Response::json([
                'error' =>
                    'max_value must be numeric'
            ], 422);

            return false;
        }

        if (
            $minValue !== null &&
            $maxValue !== null &&
            (float) $minValue >
                (float) $maxValue
        ) {

            Response::json([
                'error' =>
                    'min_value cannot exceed max_value'
            ], 422);

            return false;
        }

        return true;
    }

    private function toFloatOrNull(
        mixed $value
    ): ?float {

        return $value === null
            ? null
            : (float) $value;
    }
}