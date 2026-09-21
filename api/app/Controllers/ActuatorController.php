<?php

namespace App\Controllers;

use App\Core\Response;
use App\Repositories\ActuatorCommandRepository;
use App\Services\MqttService;

class ActuatorController
{
    public function __construct(
        private ActuatorCommandRepository $repository,
        private MqttService $mqtt
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

            if (!isset($body['device_id'])) {

                Response::json([
                    'error' => 'device_id is required'
                ], 422);

                return;
            }

            if (!isset($body['command'])) {

                Response::json([
                    'error' => 'command is required'
                ], 422);

                return;
            }

            $deviceId =
                (int) $body['device_id'];

            $command =
                strtoupper($body['command']);

            if (!in_array(
                $command,
                ['ON', 'OFF'],
                true
            )) {

                Response::json([
                    'error' => 'command must be ON or OFF'
                ], 422);

                return;
            }

            $commandRecord =
                $this->repository->create(
                    $deviceId,
                    $actuator,
                    $command
                );

            $topic =
                "ecosense/device/{$deviceId}/commands/{$actuator}";

            $this->mqtt->publish(
                $topic,
                [
                    'command_id' =>
                        (int) $commandRecord['id'],

                    'command' =>
                        $command
                ]
            );

            $this->repository->markSent(
                (int) $commandRecord['id']
            );

            Response::json([
                'message' => 'Command sent',
                'data' => [
                    'id' =>
                        (int) $commandRecord['id'],

                    'device_id' =>
                        $deviceId,

                    'actuator' =>
                        $actuator,

                    'command' =>
                        $command,

                    'status' =>
                        'sent'
                ]
            ], 201);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' => $error->getMessage()
            ], 500);
        }
    }
}