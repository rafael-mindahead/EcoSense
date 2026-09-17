<?php

namespace App\Repositories;

use PDO;

class MeansurementRepository
{
    public function __construct(
        private PDO $connection
    ) {
    }

    public function findLatest(): ?array
    {
        $sql = "
            SELECT
                id,
                device_id,
                temperature,
                humidity,
                luminosity,
                air_quality,
                created_at
            FROM meansurements
            ORDER BY created_at DESC
            LIMIT 1
        ";

        $statement = $this->connection->query($sql);

        $meansurement =
            $statement->fetch(PDO::FETCH_ASSOC);

        if (!$meansurement) {
            return null;
        }

        return [
            'id' => (int) $meansurement['id'],
            'device_id' => (int) $meansurement['device_id'],
            'temperature' => (float) $meansurement['temperature'],
            'humidity' => (float) $meansurement['humidity'],
            'luminosity' => (float) $meansurement['luminosity'],
            'air_quality' => (float) $meansurement['air_quality'],
            'created_at' => $meansurement['created_at']
        ];
    }

    public function findAll(int $limit = 50): array
    {
        $limit = max(1, min($limit, 100));

        $sql = "
            SELECT
                id,
                device_id,
                temperature,
                humidity,
                luminosity,
                air_quality,
                created_at
            FROM meansurements
            ORDER BY created_at DESC
            LIMIT {$limit}
        ";

        $statement =
            $this->connection->query($sql);

        return $statement->fetchAll(
            PDO::FETCH_ASSOC
        );
    }

    public function create(array $data): array
    {
        $sql = "
            INSERT INTO meansurements (
                device_id,
                temperature,
                humidity,
                luminosity,
                air_quality
            )
            VALUES (
                :device_id,
                :temperature,
                :humidity,
                :luminosity,
                :air_quality
            )
            RETURNING
                id,
                device_id,
                temperature,
                humidity,
                luminosity,
                air_quality,
                created_at
        ";

        $statement =
            $this->connection->prepare($sql);

        $statement->execute([
            ':device_id' => $data['device_id'],
            ':temperature' => $data['temperature'],
            ':humidity' => $data['humidity'],
            ':luminosity' => $data['luminosity'],
            ':air_quality' => $data['air_quality']
        ]);

        return $statement->fetch(
            PDO::FETCH_ASSOC
        );
    }
}