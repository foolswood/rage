#pragma once
#include "macros.h"

/*
 * Parameterisable error type.
 */
#define RAGE_OR_ERROR(t) RAGE_EITHER(char const *, t)
/*
 * Return a failure value (not sure if the return is smart).
 * et - Either type.
 * s - String describing the error.
 */
#define RAGE_FAILURE(et, s) (et) {.half=RAGE_EITHER_LEFT, .left=s}
/*
 * Return a success value (not sure if the return is smart).
 * et - Either type.
 * v - Successful return value..
 */
#define RAGE_SUCCESS(et, v) (et) {.half=RAGE_EITHER_RIGHT, .right=v}

/*
 * Boolean check for whether the error type instance e failed.
 */
#define RAGE_FAILED(e) (e.half == RAGE_EITHER_LEFT)
/*
 * Extract the failure string from the error instance e.
 * Note: if you do this on a success bad things may happen.
 */
#define RAGE_FAILURE_VALUE(e) e.left
/*
 * Extract the success value from the error instance e.
 * Note: if you do this on a failure bad things may happen.
 */
#define RAGE_SUCCESS_VALUE(e) e.right
/*
 * Cast one type of failure to another.
 * Note: don't use this on a success, bad things will happen.
 */
#define RAGE_FAILURE_CAST(et, e) RAGE_FAILURE(et, RAGE_FAILURE_VALUE(e))

/*
 * A concrete error type for things that don't return anything, but may fail.
 */
typedef RAGE_OR_ERROR(void *) rage_Error;
/*
 * Return succeeded.
 */
#define RAGE_OK RAGE_SUCCESS(rage_Error, NULL)
/*
 * Return a failure string message.
 */
#define RAGE_ERROR(msg) RAGE_FAILURE(rage_Error, msg)
