<?php

namespace App\Repositories;

use PDO;

class MeansurementRepository
{
    public function __construct(
        private PDO $connection
    ) {
    }

    public function findLatest(?int $deviceId = null): ?array
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
        ";

        $params = [];

        if ($deviceId !== null) {
            $sql .= "
                WHERE device_id = :device_id
            ";

            $params[':device_id'] = $deviceId;
        }

        $sql .= "
            ORDER BY created_at DESC
            LIMIT 1
        ";

        $statement =
            $this->connection->prepare($sql);

        $statement->execute($params);

        $row = $statement->fetch(
            PDO::FETCH_ASSOC
        );

        return $row
            ? $this->normalizeRow($row)
            : null;
    }
    public function findAll(array $filters): array
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
        ";

        $where = [];
        $params = [];

        if ($filters['device_id'] !== null) {

            $where[] =
                "device_id = :device_id";

            $params[':device_id'] =
                $filters['device_id'];
        }

        if ($filters['from'] !== null) {

            $where[] =
                "created_at >= :from";

            $params[':from'] =
                $filters['from'];
        }

        if ($filters['to'] !== null) {

            $where[] =
                "created_at <= :to";

            $params[':to'] =
                $filters['to'];
        }

        if (!empty($where)) {

            $sql .= "
                WHERE "
                . implode(
                    " AND ",
                    $where
                );
        }

        $sql .= "
            ORDER BY created_at DESC
            LIMIT {$filters['limit']}
        ";

        $statement =
            $this->connection->prepare($sql);

        $statement->execute($params);

        $rows =
            $statement->fetchAll(
                PDO::FETCH_ASSOC
            );

        return array_map(
            fn(array $row) =>
                $this->normalizeRow($row),
            $rows
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

        $row = $statement->fetch(
            PDO::FETCH_ASSOC
        );
        return $this->normalizeRow($row);
    }
    private function normalizeRow(array $row): array 
    {

    return [
        'id' =>
            (int) $row['id'],

        'device_id' =>
            (int) $row['device_id'],

        'temperature' =>
            (float) $row['temperature'],

        'humidity' =>
            (float) $row['humidity'],

        'luminosity' =>
            (float) $row['luminosity'],

        'air_quality' =>
            (float) $row['air_quality'],

        'created_at' =>
            $row['created_at']
        ];
    }
}