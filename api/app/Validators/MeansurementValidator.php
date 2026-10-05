<?php

namespace App\Validators;

class MeansurementValidator
{
    public function validate(
        array $data
    ): array {

        $errors = [];

        $requiredFields = [
            'device_id',
            'temperature',
            'humidity',
            'luminosity',
            'air_quality'
        ];

        foreach ($requiredFields as $field) {

            if (!array_key_exists(
                $field,
                $data
            )) {

                $errors[] =
                    "{$field} is required";
            }
        }

        if (!empty($errors)) {
            return $errors;
        }

        if (
            !is_numeric($data['device_id']) ||
            (int) $data['device_id'] <= 0
        ) {

            $errors[] =
                'device_id must be a positive integer';
        }

        if (
            !is_numeric(
                $data['temperature']
            )
        ) {

            $errors[] =
                'temperature must be numeric';
        }

        if (
            !is_numeric(
                $data['humidity']
            )
        ) {

            $errors[] =
                'humidity must be numeric';

        } else {

            $humidity =
                (float) $data['humidity'];

            if (
                $humidity < 0 ||
                $humidity > 100
            ) {

                $errors[] =
                    'humidity must be between 0 and 100';
            }
        }

        if (
            !is_numeric(
                $data['luminosity']
            )
        ) {

            $errors[] =
                'luminosity must be numeric';

        } elseif (
            (float) $data['luminosity'] < 0
        ) {

            $errors[] =
                'luminosity cannot be negative';
        }

        if (
            !is_numeric(
                $data['air_quality']
            )
        ) {

            $errors[] =
                'air_quality must be numeric';

        } elseif (
            (float) $data['air_quality'] < 0
        ) {

            $errors[] =
                'air_quality cannot be negative';
        }

        return $errors;
    }
}