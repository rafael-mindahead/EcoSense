<?php

namespace App\Services;

use App\Repositories\AlertRepository;

class AlertService
{
    public function __construct(
        private AlertRepository $repository
    ){   
    }
    public function list(): array 
    {
        $alerts = $this->repository->findAll();
        return [
            'count' => count($alerts),
            'data' => $alerts
        ];
    }
    public function resolve(mixed $alertId): array
    {
        $alertId = $this->normalizeId($alertId);
        $alert = $this->repository->resolve($alertId);
        if (!$alert){
            throw new \RuntimeException('alerta nao encontrado ou ja resolvido');
        }
        return $alert;
    }
    public function normalizeId(mixed $alertId): int
    {
        if (!is_numeric($alertId) || (int) $alertId <= 0){
            throw new \InvalidArgumentException('alerta id invalido');
        }
        return (int) $alertId;
    }
    public function create(
        int $deviceId,
        string $metric,
        float $value,
        string $message
    ): array
    {
        if ($deviceId <= 0){
            throw new \InvalidArgumentException('device_id invalido');
        }
        if ($metric === ''){
            throw new \InvalidArgumentException('metric esta vazio');
        }
        if ($message === ''){
            throw new \InvalidArgumentException('message esta vazio');
        }
        return $this->repository->create(
            $deviceId,
            $metric,
            $value,
            $message
        );
    }
}