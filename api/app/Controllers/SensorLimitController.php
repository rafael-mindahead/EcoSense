<?php

namespace App\Controllers;

use App\Core\Response;
use App\Services\SensorLimitService;

class SensorLimitController
{
    public function __construct(
        private SensorLimitService $service
    ) {
    }

    public function index(): void
    {
        try {

            Response::json(
                $this->service->list(
                    $_GET['device_id']
                    ?? null
                )
            );

        } catch (
            \InvalidArgumentException $error
        ) {

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

            $body = $this->getBody();

            if ($body === null) {
                return;
            }

            $limit =
                $this->service
                    ->create($body);

            Response::json([
                'message' =>
                    'Sensor limit created',
                'data' => $limit
            ], 201);

        } catch (
            \InvalidArgumentException $error
        ) {

            Response::json([
                'error' =>
                    $error->getMessage()
            ], 422);

        } catch (
            \DomainException $error
        ) {

            Response::json([
                'error' =>
                    $error->getMessage()
            ], 409);

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

    public function update(): void
    {
        try {

            $body = $this->getBody();

            if ($body === null) {
                return;
            }

            $limit =
                $this->service
                    ->update($body);

            Response::json([
                'message' =>
                    'Sensor limit updated',
                'data' => $limit
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

    private function getBody(): ?array
    {
        $body = json_decode(
            file_get_contents(
                'php://input'
            ),
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
}