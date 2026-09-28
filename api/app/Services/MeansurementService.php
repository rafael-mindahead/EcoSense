<?php

namespace App\Services;

use App\Repositories\MeansurementRepository;
use App\Repositories\DeviceRepository;
use App\Repositories\SensorLimitRepository;
use App\Repositories\AlertRepository;

class MeansurementService
{
    public function __construct(
        private MeansurementRepository $meansurementRepository,
        private DeviceRepository $deviceRepository,
        private SensorLimitRepository $sensorLimitRepository,
        private AlertRepository $alertRepository
    ) {
    }

    public function create(
        array $data
    ): array {

        $meansurement =
            $this->meansurementRepository
                ->create($data);

        $this->deviceRepository
            ->markOnline(
                (int) $data['device_id']
            );

        $alerts =
            $this->checkLimits($data);

        return [
            'meansurement' => $meansurement,
            'alerts' => $alerts
        ];
    }

    private function checkLimits(
        array $data
    ): array {

        $deviceId =
            (int) $data['device_id'];

        $limits =
            $this->sensorLimitRepository
                ->findEnabledByDevice(
                    $deviceId
                );

        $alerts = [];

        foreach ($limits as $limit) {

            $metric = $limit['metric'];

            if (!array_key_exists(
                $metric,
                $data
            )) {
                continue;
            }

            $value =
                (float) $data[$metric];

            $minValue =
                $limit['min_value'] !== null
                    ? (float) $limit['min_value']
                    : null;

            $maxValue =
                $limit['max_value'] !== null
                    ? (float) $limit['max_value']
                    : null;

            if (
                $minValue !== null &&
                $value < $minValue
            ) {

                $alerts[] =
                    $this->alertRepository
                        ->create(
                            $deviceId,
                            $metric,
                            $value,
                            "{$metric} abaixo do limite minimo"
                        );

                continue;
            }

            if (
                $maxValue !== null &&
                $value > $maxValue
            ) {

                $alerts[] =
                    $this->alertRepository
                        ->create(
                            $deviceId,
                            $metric,
                            $value,
                            "{$metric} acima do limite maximo"
                        );
            }
        }

        return $alerts;
    }
}