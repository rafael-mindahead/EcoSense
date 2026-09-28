CREATE TABLE IF NOT EXISTS sensor_limits (
    id BIGSERIAL PRIMARY KEY,

    device_id INTEGER NOT NULL,

    metric VARCHAR(50) NOT NULL,

    min_value NUMERIC(10, 2),

    max_value NUMERIC(10, 2),

    enabled BOOLEAN NOT NULL DEFAULT TRUE,

    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT fk_sensor_limits_device
        FOREIGN KEY (device_id)
        REFERENCES devices(id)
        ON DELETE CASCADE,

    CONSTRAINT uq_sensor_limits_device_metric
        UNIQUE (device_id, metric)
);