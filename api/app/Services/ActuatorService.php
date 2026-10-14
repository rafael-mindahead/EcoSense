<?php

namespace App\Services;

use App\Repositories\ActuatorCommandRepository;

class ActuatorService
{
    private const ALLOWED_ACTUATORS = [
        'fan',
        'exhaust'
    ];

    private const ALLOWED_COMMANDS = [
        'ON',
        'OFF'
    ];

    public function __construct(
        private ActuatorCommandRepository $repository,
        private MqttService $mqtt,
        private DeviceService $deviceService
    ) {
    }

    public function control(
        string $actuator,
        array $data
    ): array {

        $actuator = strtolower($actuator);

        if (!in_array(
            $actuator,
            self::ALLOWED_ACTUATORS,
            true
        )) {
            throw new \InvalidArgumentException(
                'invalid actuator'
            );
        }

        if (!isset($data['device_id'])) {
            throw new \InvalidArgumentException(
                'device_id is required'
            );
        }

        if (!isset($data['command'])) {
            throw new \InvalidArgumentException(
                'command is required'
            );
        }

        $deviceId =
            $this->normalizeDeviceId(
                $data['device_id']
            );

        $command =
            strtoupper(
                (string) $data['command']
            );

        if (!in_array(
            $command,
            self::ALLOWED_COMMANDS,
            true
        )) {
            throw new \InvalidArgumentException(
                'command must be ON or OFF'
            );
        }

        $this->deviceService
            ->getById($deviceId);

        $commandRecord =
            $this->repository->create(
                $deviceId,
                $actuator,
                $command
            );

        $commandId =
            (int) $commandRecord['id'];

        $topic =
            "ecosense/device/{$deviceId}/commands/{$actuator}";

        $this->mqtt->publish(
            $topic,
            [
                'command_id' => $commandId,
                'command' => $command
            ]
        );

        $this->repository
            ->markSent($commandId);

        return [
            'id' => $commandId,
            'device_id' => $deviceId,
            'actuator' => $actuator,
            'command' => $command,
            'status' => 'sent'
        ];
    }

    public function acknowledge(
        mixed $commandId
    ): array {

        $commandId =
            $this->normalizeCommandId(
                $commandId
            );

        $command =
            $this->repository
                ->markExecuted(
                    $commandId
                );

        if (!$command) {
            throw new \RuntimeException(
                'command not found or already executed'
            );
        }

        return $command;
    }

    private function normalizeDeviceId(
        mixed $deviceId
    ): int {

        if (
            !is_numeric($deviceId) ||
            (int) $deviceId <= 0
        ) {
            throw new \InvalidArgumentException(
                'device_id must be a positive integer'
            );
        }

        return (int) $deviceId;
    }

    private function normalizeCommandId(
        mixed $commandId
    ): int {

        if (
            !is_numeric($commandId) ||
            (int) $commandId <= 0
        ) {
            throw new \InvalidArgumentException(
                'command_id must be a positive integer'
            );
        }

        return (int) $commandId;
    }
}