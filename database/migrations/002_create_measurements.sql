CREATE TABLE IF NOT EXISTS meansurements(
    --RELACAO 1N COM DEVICES --
    id BIGSERIAL PRIMARY KEY,
    device_id INTEGER NOT NULL,
    temperature NUMERIC(5, 2) NOT NULL,
    humidity NUMERIC(5, 2) NOT NULL,
    luminosity NUMERIC(10,2) NOT NULL,
    air_quality NUMERIC(10,2) NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_meansurements_device
        FOREIGN KEY (device_id)
        REFERENCES devices(id)
        ON DELETE CASCADE
);
CREATE INDEX IF NOT EXISTS idx_meansurements_device_created_at
ON meansurements(device_id, created_at DESC);