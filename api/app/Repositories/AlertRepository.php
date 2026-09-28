<?php

namespace App\Repositories;

use PDO;

class AlertRepository
{
    public function __construct(
        private PDO $connection
    ) {
    }

    public function create(
        int $deviceId,
        string $metric,
        float $value,
        string $message
    ): array {

        $sql = "
            INSERT INTO alerts (
                device_id,
                metric,
                value,
                message
            )
            VALUES (
                :device_id,
                :metric,
                :value,
                :message
            )
            RETURNING *
        ";

        $statement =
            $this->connection->prepare($sql);

        $statement->execute([
            ':device_id' => $deviceId,
            ':metric' => $metric,
            ':value' => $value,
            ':message' => $message
        ]);

        return $statement->fetch(
            PDO::FETCH_ASSOC
        );
    }

    public function findAll(): array
    {
        $sql = "
            SELECT
                id,
                device_id,
                metric,
                value,
                message,
                status,
                created_at,
                resolved_at
            FROM alerts
            ORDER BY created_at DESC
        ";

        $statement =
            $this->connection->query($sql);

        return $statement->fetchAll(
            PDO::FETCH_ASSOC
        );
    }
    public function resolve(int $id): ?array
    {
        $sql = "
            UPDATE alerts
            SET
                status = 'resolved',
                resolved_at = CURRENT_TIMESTAMP
            WHERE id = :id
            AND status = 'open'
            RETURNING *
        ";

        $statement =
            $this->connection->prepare($sql);

        $statement->execute([
            ':id' => $id
        ]);

        $alert =
            $statement->fetch(
                PDO::FETCH_ASSOC
            );

        return $alert ?: null;
    }
}