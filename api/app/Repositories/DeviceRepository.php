<?php

namespace App\Repositories;

use PDO;

class DeviceRepository
{
    public function __construct(
        private PDO $connection
    ) {
    }

    public function findAll(): array
    {
        $sql = "
            SELECT
                id,
                name,
                device_code,
                status,
                last_seen,
                created_at
            FROM devices
            ORDER BY id
        ";

        $statement = $this->connection->query($sql);

        return $statement->fetchAll(
            PDO::FETCH_ASSOC
        );
    }
}