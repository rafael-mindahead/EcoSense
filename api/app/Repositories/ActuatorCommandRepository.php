<?php

namespace App\Repositories;

use PDO;

class ActuatorCommandRepository
{
    public function __construct(
        private PDO $connection
    ) {
    }

    public function create(
        int $deviceId,
        string $actuator,
        string $command
    ): array {
        $sql = "
            INSERT INTO actuator_commands (
                device_id,
                actuator,
                command
            )
            VALUES (
                :device_id,
                :actuator,
                :command
            )
            RETURNING *
        ";

        $statement = $this->connection->prepare($sql);

        $statement->execute([
            ':device_id' => $deviceId,
            ':actuator' => $actuator,
            ':command' => $command
        ]);

        return $statement->fetch(
            PDO::FETCH_ASSOC
        );
    }

    public function markSent(int $id): void
    {
        $sql = "
            UPDATE actuator_commands
            SET status = 'sent'
            WHERE id = :id
        ";

        $statement = $this->connection->prepare($sql);

        $statement->execute([
            ':id' => $id
        ]);
    }
    public function markExecuted(int $id): ?array
    {
        $sql = "
            UPDATE actuator_commands
            SET
                status = 'executed',
                executed_at = CURRENT_TIMESTAMP
            WHERE id = :id
            RETURNING *
        ";

        $statement = $this->connection->prepare($sql);

        $statement->execute([
            ':id' => $id
        ]);

        $command = $statement->fetch(
            PDO::FETCH_ASSOC
        );

        return $command ?: null;
    }
}