<?php

namespace App\Services;

use App\Repositories\SensorLimitRepository;

class SensorLimitService
{
    private const ALLOWED_METRICS = [
        'temperature',
        'humidity',
        'luminosity',
        'air_quality'
    ];

    public function __construct(
        private SensorLimitRepository $repository,
        private DeviceService $deviceService
    ) {
    }

    public function list(
        mixed $deviceId = null
    ): array {

        if (
            $deviceId !== null &&
            $deviceId !== ''
        ) {
            $deviceId =
                $this->normalizeDeviceId(
                    $deviceId
                );
        } else {
            $deviceId = null;
        }

        $limits =
            $this->repository
                ->findAll($deviceId);

        return [
            'count' => count($limits),
            'data' => $limits
        ];
    }

    public function create(
        array $data
    ): array {

        $data =
            $this->normalizeData($data);

        // garante que o device existe
        $this->deviceService
            ->getById(
                $data['device_id']
            );

        $limit =
            $this->repository->create(
                $data['device_id'],
                $data['metric'],
                $data['min_value'],
                $data['max_value'],
                $data['enabled']
            );

        if (!$limit) {
            throw new \DomainException(
                'Sensor limit already exists'
            );
        }

        return $limit;
    }

    public function update(
        array $data
    ): array {

        $data =
            $this->normalizeData($data);

        $this->deviceService
            ->getById(
                $data['device_id']
            );

        $limit =
            $this->repository->update(
                $data['device_id'],
                $data['metric'],
                $data['min_value'],
                $data['max_value'],
                $data['enabled']
            );

        if (!$limit) {
            throw new \RuntimeException(
                'Sensor limit not found'
            );
        }

        return $limit;
    }

    private function normalizeData(
        array $data
    ): array {

        if (
            !isset(
                $data['device_id'],
                $data['metric']
            )
        ) {
            throw new \InvalidArgumentException(
                'device_id and metric are required'
            );
        }

        $deviceId =
            $this->normalizeDeviceId(
                $data['device_id']
            );

        $metric =
            (string) $data['metric'];

        if (!in_array(
            $metric,
            self::ALLOWED_METRICS,
            true
        )) {
            throw new \InvalidArgumentException(
                'metric invalida'
            );
        }

        $minValue =
            $this->normalizeNumber(
                $data['min_value'] ?? null,
                'min_value'
            );

        $maxValue =
            $this->normalizeNumber(
                $data['max_value'] ?? null,
                'max_value'
            );

        if (
            $minValue === null &&
            $maxValue === null
        ) {
            throw new \InvalidArgumentException(
                'min_value or max_value is required'
            );
        }

        if (
            $minValue !== null &&
            $maxValue !== null &&
            $minValue > $maxValue
        ) {
            throw new \InvalidArgumentException(
                'min_value cannot exceed max_value'
            );
        }

        return [
            'device_id' => $deviceId,
            'metric' => $metric,
            'min_value' => $minValue,
            'max_value' => $maxValue,
            'enabled' =>
                $this->normalizeEnabled(
                    $data['enabled'] ?? true
                )
        ];
    }

    private function normalizeDeviceId(
        mixed $deviceId
    ): int {

        if (
            !is_numeric($deviceId) ||
            (int) $deviceId <= 0
        ) {
            throw new \InvalidArgumentException(
                'device_id invalido'
            );
        }

        return (int) $deviceId;
    }

    private function normalizeNumber(
        mixed $value,
        string $field
    ): ?float {

        if ($value === null) {
            return null;
        }

        if (!is_numeric($value)) {
            throw new \InvalidArgumentException(
                "{$field} must be numeric"
            );
        }

        return (float) $value;
    }

    private function normalizeEnabled(
        mixed $value
    ): bool {

        if (is_bool($value)) {
            return $value;
        }

        if (
            $value === 1 ||
            $value === '1' ||
            $value === 'true'
        ) {
            return true;
        }

        if (
            $value === 0 ||
            $value === '0' ||
            $value === 'false'
        ) {
            return false;
        }

        throw new \InvalidArgumentException(
            'enabled must be boolean'
        );
    }
}