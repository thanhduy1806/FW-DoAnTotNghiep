/*
 * csp_heap.h
 */

#ifndef CSP_HEAP_H
#define CSP_HEAP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void * pvCSPMalloc( size_t xWantedSize );
void vCSPFree( void * pv );
size_t xCSPGetFreeHeapSize( void );
size_t xCSPGetMinimumEverFreeHeapSize( void );

#ifdef __cplusplus
}
#endif

#endif /* CSP_HEAP_H */