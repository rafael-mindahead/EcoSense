CREATE TABLE IF NOT EXISTS alerts (
    id BIGSERIAL PRIMARY KEY,

    device_id INTEGER NOT NULL,

    metric VARCHAR(50) NOT NULL,

    value NUMERIC(10, 2) NOT NULL,

    message VARCHAR(255) NOT NULL,

    status VARCHAR(20) NOT NULL DEFAULT 'open',

    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,

    resolved_at TIMESTAMPTZ,

    CONSTRAINT fk_alert_device
        FOREIGN KEY (device_id)
        REFERENCES devices(id)
        ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_alerts_device_created_at
ON alerts(device_id, created_at DESC);