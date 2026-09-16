<?php

namespace App\Core;

class Router
{
    private array $routes = [];

    public function get(string $path, callable $handler): void
    {
        $this->routes['GET'][$path] = $handler;
    }
    public function post(string $path, callable $handler): void
    {
        $this->routes['POST'][$path] = $handler;
    }

    public function dispatch(): void
    {
        $method = $_SERVER['REQUEST_METHOD'];

        $path = parse_url(
            $_SERVER['REQUEST_URI'],
            PHP_URL_PATH
        );

        if (isset($this->routes[$method][$path])) {

            $handler = $this->routes[$method][$path];

            $handler();

            return;
        }

        Response::json(
            [
                'error' => 'Route not found'
            ],
            404
        );
    }
}