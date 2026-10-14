<?php

namespace App\Controllers;

use App\Core\Response;
use App\Services\ActuatorService;

class ActuatorController
{
    public function __construct(
        private ActuatorService $service
    ) {
    }

    public function control(
        string $actuator
    ): void {

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

            $result =
                $this->service->control(
                    $actuator,
                    $body
                );

            Response::json([
                'message' => 'Command sent',
                'data' => $result
            ], 201);

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