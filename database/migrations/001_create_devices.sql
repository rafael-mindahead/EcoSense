CREATE TABLE IF NOT EXISTS devices (
    -- ve status do esp --
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    device_code VARCHAR(100) NOT NULL UNIQUE,
    status VARCHAR(20) NOT NULL DEFAULT 'offline',
    last_seen TIMESTAMPTZ,
    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP
);
