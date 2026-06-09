/* Copyright (c) 2013-2017 GomSpace A/S. All rights reserved. */

#include <gs_support/mutex.h>

/* ============================================================================
 *  Linux / POSIX implementation
 * ============================================================================ */
#if defined(__linux__)

#include <errno.h>
#include <stdlib.h>

gs_error_t gs_mutex_create(gs_mutex_t *mutex)
{
    if (mutex == NULL) return GS_ERROR_ARG;

    *mutex = malloc(sizeof(pthread_mutex_t));
    if (*mutex == NULL) return GS_ERROR_ALLOC;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    int res = pthread_mutex_init(*mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    if (res != 0) {
        free(*mutex);
        *mutex = NULL;
        return gs_error(errno);
    }
    return GS_OK;
}

gs_error_t gs_mutex_destroy(gs_mutex_t mutex)
{
    if (mutex == NULL) return GS_OK;
    int res = pthread_mutex_destroy(mutex);
    free(mutex);
    return (res == 0) ? GS_OK : gs_error(errno);
}

gs_error_t gs_mutex_lock(gs_mutex_t mutex)
{
    if (mutex == NULL) return GS_ERROR_ARG;
    int res = pthread_mutex_lock(mutex);
    return (res == 0) ? GS_OK : gs_error(errno);
}

gs_error_t gs_mutex_unlock(gs_mutex_t mutex)
{
    if (mutex == NULL) return GS_ERROR_ARG;
    int res = pthread_mutex_unlock(mutex);
    return (res == 0) ? GS_OK : gs_error(errno);
}

/* ============================================================================
 *  FreeRTOS implementation
 * ============================================================================ */
#else

/* Timeout used for gs_mutex_lock().
 * portMAX_DELAY = block forever (matches pthread behaviour). */
#define MUTEX_LOCK_TIMEOUT_TICKS    portMAX_DELAY

gs_error_t gs_mutex_create(gs_mutex_t *mutex)
{
    if (mutex == NULL) return GS_ERROR_ARG;

    *mutex = xSemaphoreCreateRecursiveMutex();
    if (*mutex == NULL) return GS_ERROR_ALLOC;   /* FreeRTOS heap full */

    return GS_OK;
}

gs_error_t gs_mutex_destroy(gs_mutex_t mutex)
{
    if (mutex == NULL) return GS_OK;
    vSemaphoreDelete(mutex);
    return GS_OK;
}

gs_error_t gs_mutex_lock(gs_mutex_t mutex)
{
    if (mutex == NULL) return GS_ERROR_ARG;

    if (xSemaphoreTakeRecursive(mutex, MUTEX_LOCK_TIMEOUT_TICKS) == pdTRUE) {
        return GS_OK;
    }
    return GS_ERROR_TIMEOUT;
}

gs_error_t gs_mutex_unlock(gs_mutex_t mutex)
{
    if (mutex == NULL) return GS_ERROR_ARG;

    if (xSemaphoreGiveRecursive(mutex) == pdTRUE) {
        return GS_OK;
    }
    return GS_ERROR_UNKNOWN;
}

#endif /* __linux__ */