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

            CASE
                WHEN last_seen  IS NOT NULL
                AND last_seen >= 
                     CURRENT_TIMESTAMP
                     - INTERVAL '30 seconds'
                THEN 'online'
                ELSE 'offline'
            END AS status,
            last_seen,
            CASE
                WHEN last_seen IS NULL
                THEN NULL

                ELSE EXTRACT(
                    EPOCH FROM (CURRENT_TIMESTAMP - last_seen)
                )::INTEGER
            END AS seconds_since_last_seen,

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
    public function findById(int $deviceId): ?array
    {
        $sql = "
        SELECT 
            id,
            name,
            device_code,
            CASE
                WHEN last_seen IS NOT NULL
                AND last_seen >=
                    CURRENT_TIMESTAMP - INTERVAL '30 seconds'
                THEN 'online'
                ELSE 'offline'
            END AS status,
            last_seen,
            CASE
                WHEN last_seen IS NULL
                THEN NULL
                ELSE EXTRACT(
                    EPOCH FROM (CURRENT_TIMESTAMP - last_seen)
                )::INTEGER
            END AS seconds_since_last_seen,
            created_at
        FROM devices
        WHERE id = :device_id
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