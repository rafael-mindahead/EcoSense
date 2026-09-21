<?php

namespace App\Repositores;

use PDO;

class ActuatorCommandRepository
{
    public function __construct(private PDO $connection)
    {
        public function create(
            int $deviceId,
            string $actuador,
            string $command
        ): array {
            $sql = "
                INSERT INTO actuator_commands (
                    device_id,
                    actuador,
                    command
                )
                VALUES (
                    :device_id,
                    :actuador,
                    :command
                )
                RETURNING *
            ";
            $statement = $this->connection->prepare($sql);
            $statement->execute([
                ':device_id' => $deviceId,
                ':actuador' => $actuador,
                ':command' => $command
            ])
            return $statement->(
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
            statement = $this->connection->prepare($sql);
            $statement->execute([
                ':id' => $id
            ]);
        }
    }
}