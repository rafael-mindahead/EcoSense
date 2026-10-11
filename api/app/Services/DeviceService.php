<?php
namespace App\Services;

use App\Repositories\DeviceRepository;

class DeviceService
{
    public function __construct(
        private DeviceRepository $repository
    ){
    }
    public function list(): array
    {
        $devices = $this->repository->findAll();
        return [
            'count' => count($devices),
            'data' => $devices
        ];
    }
    public function getById(mixed $deviceId): array
    {
        $deviceId = $this->validateId($deviceId);
        $device = $this->repository->findById($deviceId);
        if (!$device) {
            throw new \RuntimeException('device not found');
        }
        return $device;
    }
    private function validateId(mixed $deviceId): int
    {
        if (!is_numeric($deviceId) || (int) $deviceId <= 0) {
            throw new \InvalidArgumentException('device_id must be a positive integer');
        }
        return (int) $deviceId;
    }
}
