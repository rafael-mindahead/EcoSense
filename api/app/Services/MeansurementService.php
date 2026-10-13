<?php

namespace App\Services;

use App\Repositories\MeansurementRepository;
use App\Repositories\DeviceRepository;
use App\Repositories\SensorLimitRepository;
use App\Validators\MeansurementValidator;

class MeansurementService
{
    public function __construct(
        private MeansurementRepository $meansurementRepository,
        private DeviceRepository $deviceRepository,
        private SensorLimitRepository $sensorLimitRepository,
        private AlertService $alertService,
        private MeansurementValidator $validator
    ) {
    }

    public function create(
        array $data
    ): array {
        $errors = $this->validator->validate($data);

        if (!empty($errors)) {
            throw new \invalidArgumentException(implode(',', $errors));
        }

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
                    $this->alertService
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
                    $this->alertService
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
    public function latest(mixed $deviceId = null): ?array
    {
        if ($deviceId !== null && (!is_numeric($deviceId) || (int) $deviceId <= 0)){
        
            throw new \InvalidArgumentException('Invalid device id');
        }
        return $this->meansurementRepository->findLatest($deviceId !== null ? (int) $deviceId : null);
    }
    public function history(array $filters): array 
    {
        $filters =
            $this->normalizeHistoryFilters(
                $filters
            );

        $data =
            $this->meansurementRepository
                ->findAll($filters);

        return [
            'count' => count($data),
            'filters' => $filters,
            'data' => $data
        ];
    }
    private function normalizeHistoryFilters(array $filters): array 
    {

        $deviceId =
            $filters['device_id']
            ?? null;

        if (
            $deviceId !== null &&
            $deviceId !== ''
        ) {

            if (
                !is_numeric($deviceId) ||
                (int) $deviceId <= 0
            ) {
                throw new \InvalidArgumentException(
                    'device_id must be a positive integer'
                );
            }

            $deviceId =
                (int) $deviceId;

        } else {
            $deviceId = null;
        }


        $limit =
            $filters['limit']
            ?? 50;

        if (
            !is_numeric($limit) ||
            (int) $limit < 1 ||
            (int) $limit > 100
        ) {
            throw new \InvalidArgumentException(
                'limit must be between 1 and 100'
            );
        }

        $limit = (int) $limit;


        $from =
            $this->normalizeDate(
                $filters['from']
                ?? null,
                false
            );

        $to =
            $this->normalizeDate(
                $filters['to']
                ?? null,
                true
            );


        if (
            $from !== null &&
            $to !== null &&
            strtotime($from) >
            strtotime($to)
        ) {
            throw new \InvalidArgumentException(
                'from cannot be greater than to'
            );
        }


        return [
            'device_id' => $deviceId,
            'from' => $from,
            'to' => $to,
            'limit' => $limit
        ];
    }
    private function normalizeDate(mixed $value,bool $endOfDay): ?string 
    {

        if (
            $value === null ||
            $value === ''
        ) {
            return null;
        }

        $value = (string) $value;

        if (
            preg_match(
                '/^\d{4}-\d{2}-\d{2}$/',
                $value
            )
        ) {
            return $value
                . (
                    $endOfDay
                        ? ' 23:59:59'
                        : ' 00:00:00'
                );
        }

        try {

            $date =
                new \DateTimeImmutable(
                    $value
                );

            return $date->format(
                'Y-m-d H:i:sP'
            );

        } catch (\Throwable) {

            throw new \InvalidArgumentException(
                'invalid date format'
            );
        }
    }   
}