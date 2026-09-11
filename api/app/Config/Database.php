<?php

namespace App\Config;

use PDO;

class Database
{
    public static function connect(): PDO
    {
        $host = getenv('DB_HOST');
        $port = getenv('DB_PORT');
        $name = getenv('DB_NAME');
        $user = getenv('DB_USER');
        $password = getenv('DB_PASSWORD');

        $dsn = 
        "pgsql:host=$host;
        port=$port;
        dbname=$name";

        return new PDO(
            $dsn,
            $user,
            $password,
            [
                PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION
            ]
        );
    }
}