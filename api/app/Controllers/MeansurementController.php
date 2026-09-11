<?php

namespace App\Controllers;

use App\Core\Response;
use App\Repositories\MeansurementRepository;

class MeansurementController
{
    public function __construct(
        private MeansurementRepository $repository
    ) {
    }

    public function latest(): void
    {
        try {

            $meansurement = $this->repository->findLatest();

            if (!$meansurement) {

                Response::json([
                    'error' => 'No measurements found'
                ], 404);

                return;
            }

            Response::json($meansurement);

        } catch (\Throwable $error) {

            Response::json([
                'status' => 'error',
                'message' => $error->getMessage()
            ], 500);
        }
    }
}