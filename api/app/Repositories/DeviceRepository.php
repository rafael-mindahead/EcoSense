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
    public function markOnline(int $deviceId): ?array
    {
        $sql = "
            UPDATE devices
            SET 
            status = 'online',
            last_seen = CURRENT_TIMESTAMP
            WHERE id = :device_id
            RETURNING 
                id,
                name,
                device_code,
                status,
                last_seen
        ";

        $statement = $this->connection->prepare($sql);
        $statement->execute([
            ':device_id' => $deviceId
        ]);
        $device = $statement->fetch(
            PDO::FETCH_ASSOC
        );

        return $device ?: null;
    }
}