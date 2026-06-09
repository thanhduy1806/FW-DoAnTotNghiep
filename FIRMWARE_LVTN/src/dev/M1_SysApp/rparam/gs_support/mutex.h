#ifndef GS_UTIL_MUTEX_H
#define GS_UTIL_MUTEX_H
/* Copyright (c) 2013-2017 GomSpace A/S. All rights reserved. */
/**
   @file
   Recursive mutex — wraps pthread (Linux) or FreeRTOS sempahore.
   Cannot be used from an ISR.
*/

#include <gs_support/gs_error.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__linux__)
/* ---- Linux: thin wrapper over pthread recursive mutex ---- */
#include <pthread.h>
typedef pthread_mutex_t * gs_mutex_t;

#else
/* ---- FreeRTOS: opaque handle (SemaphoreHandle_t internally) ---- */
#include "FreeRTOS.h"
#include "semphr.h"

/* Forward-declared opaque struct keeps type safety without
 * exposing SemaphoreHandle_t in every header that includes mutex.h */
typedef SemaphoreHandle_t gs_mutex_t;

#endif /* __linux__ */

/**
   Create a recursive mutex.
   @param[out] mutex  returned handle.
   @return GS_OK on success.
*/
gs_error_t gs_mutex_create(gs_mutex_t *mutex);

/**
   Destroy mutex and free resources.
   @param[in] mutex handle.
   @return GS_OK on success.
*/
gs_error_t gs_mutex_destroy(gs_mutex_t mutex);

/**
   Lock (take) mutex — blocks until available.
   @param[in] mutex handle.
   @return GS_OK on success.
*/
gs_error_t gs_mutex_lock(gs_mutex_t mutex);

/**
   Unlock (give) mutex.
   @param[in] mutex handle.
   @return GS_OK on success.
*/
gs_error_t gs_mutex_unlock(gs_mutex_t mutex);

#ifdef __cplusplus
}
#endif
#endif /* GS_UTIL_MUTEX_H */