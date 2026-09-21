CREATE TABLE IF NOT EXISTS actuator_commands (
    id BIGSERIAL PRIMARY KEY,

    device_id INTEGER NOT NULL,

    actuator VARCHAR(30) NOT NULL,

    command VARCHAR(10) NOT NULL,

    status VARCHAR(20) NOT NULL DEFAULT 'pending',

    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,

    executed_at TIMESTAMPTZ,

    CONSTRAINT fk_actuator_command_device
        FOREIGN KEY (device_id)
        REFERENCES devices(id)
        ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_actuator_commands_device
ON actuator_commands(device_id, created_at DESC);