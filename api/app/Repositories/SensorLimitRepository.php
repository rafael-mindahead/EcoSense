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
                enabled
            FROM sensor_limits
            WHERE device_id = :device_id
            AND enabled = TRUE
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
}