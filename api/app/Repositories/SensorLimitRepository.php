<?php

namespace App\Repositories;

use PDO;

class SensorLimitRepository
{
    public function __construct(
        private PDO $connection
    ) {
    }

    public function findEnabledByDevice(
        int $deviceId
    ): array {

        $sql = "
            SELECT
                id,
                device_id,
                metric,
                min_value,
                max_value,
                enabled,
                created_at
            FROM sensor_limits
            WHERE device_id = :device_id
            AND enabled = TRUE
            ORDER BY metric
        ";

        $statement =
            $this->connection->prepare($sql);

        $statement->execute([
            ':device_id' => $deviceId
        ]);

        return $statement->fetchAll(
            PDO::FETCH_ASSOC
        );
    }

    public function findAll(
        ?int $deviceId = null
    ): array {

        if ($deviceId !== null) {

            $sql = "
                SELECT
                    id,
                    device_id,
                    metric,
                    min_value,
                    max_value,
                    enabled,
                    created_at
                FROM sensor_limits
                WHERE device_id = :device_id
                ORDER BY metric
            ";

            $statement =
                $this->connection->prepare($sql);

            $statement->execute([
                ':device_id' => $deviceId
            ]);

        } else {

            $sql = "
                SELECT
                    id,
                    device_id,
                    metric,
                    min_value,
                    max_value,
                    enabled,
                    created_at
                FROM sensor_limits
                ORDER BY device_id, metric
            ";

            $statement =
                $this->connection->query($sql);
        }

        return $statement->fetchAll(
            PDO::FETCH_ASSOC
        );
    }

    public function create(
        int $deviceId,
        string $metric,
        ?float $minValue,
        ?float $maxValue,
        bool $enabled
    ): ?array {

        $sql = "
            INSERT INTO sensor_limits (
                device_id,
                metric,
                min_value,
                max_value,
                enabled
            )
            VALUES (
                :device_id,
                :metric,
                :min_value,
                :max_value,
                :enabled
            )
            ON CONFLICT (device_id, metric)
            DO NOTHING
            RETURNING *
        ";

        $statement =
            $this->connection->prepare($sql);

        $statement->execute([
            ':device_id' => $deviceId,
            ':metric' => $metric,
            ':min_value' => $minValue,
            ':max_value' => $maxValue,
            ':enabled' => $enabled
                ? 'true'
                : 'false'
        ]);

        $limit =
            $statement->fetch(
                PDO::FETCH_ASSOC
            );

        return $limit ?: null;
    }

    public function update(
        int $deviceId,
        string $metric,
        ?float $minValue,
        ?float $maxValue,
        bool $enabled
    ): ?array {

        $sql = "
            UPDATE sensor_limits
            SET
                min_value = :min_value,
                max_value = :max_value,
                enabled = :enabled
            WHERE device_id = :device_id
            AND metric = :metric
            RETURNING *
        ";

        $statement =
            $this->connection->prepare($sql);

        $statement->execute([
            ':device_id' => $deviceId,
            ':metric' => $metric,
            ':min_value' => $minValue,
            ':max_value' => $maxValue,
            ':enabled' => $enabled
                ? 'true'
                : 'false'
        ]);

        $limit =
            $statement->fetch(
                PDO::FETCH_ASSOC
            );

        return $limit ?: null;
    }
}