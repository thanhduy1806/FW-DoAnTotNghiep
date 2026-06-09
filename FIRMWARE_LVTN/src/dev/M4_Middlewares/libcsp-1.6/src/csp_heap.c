#include "csp/csp_heap.h"
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE
#include "FreeRTOS.h"
#include "task.h"
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include <string.h>
#include <stdint.h>

#define CSP_HEAP_TOTAL_SIZE     ( 64U * 1024U )    /* 64 KB */
#define CSP_HEAP_BITS_PER_BYTE  ( ( size_t ) 8 )

static uint8_t ucCSPHeap[ CSP_HEAP_TOTAL_SIZE ];


typedef struct A_BLOCK_LINK
{
    struct A_BLOCK_LINK * pxNextFreeBlock;
    size_t                xBlockSize;
} BlockLink_t;

#define CSP_BYTE_ALIGNMENT       ( 8U )
#define CSP_BYTE_ALIGNMENT_MASK  ( CSP_BYTE_ALIGNMENT - 1U )

static const size_t xCSPHeapStructSize =
    ( sizeof( BlockLink_t ) + ( ( size_t )( CSP_BYTE_ALIGNMENT - 1 ) ) )
    & ~( ( size_t ) CSP_BYTE_ALIGNMENT_MASK );

#define CSP_MINIMUM_BLOCK_SIZE   ( ( size_t )( xCSPHeapStructSize << 1 ) )

#define CSP_BLOCK_ALLOCATED_BITMASK \
    ( ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * CSP_HEAP_BITS_PER_BYTE ) - 1 ) )

#define cspBLOCK_SIZE_IS_VALID( xBlockSize ) \
    ( ( ( xBlockSize ) & CSP_BLOCK_ALLOCATED_BITMASK ) == 0 )
#define cspBLOCK_IS_ALLOCATED( pxBlock ) \
    ( ( ( pxBlock )->xBlockSize & CSP_BLOCK_ALLOCATED_BITMASK ) != 0 )
#define cspALLOCATE_BLOCK( pxBlock ) \
    ( ( pxBlock )->xBlockSize |= CSP_BLOCK_ALLOCATED_BITMASK )
#define cspFREE_BLOCK( pxBlock ) \
    ( ( pxBlock )->xBlockSize &= ~CSP_BLOCK_ALLOCATED_BITMASK )


static BlockLink_t   xCSPStart;
static BlockLink_t * pxCSPEnd           = NULL;
static size_t        xCSPFreeBytes      = ( size_t ) 0;
static size_t        xCSPMinFreeBytes   = ( size_t ) 0;

/* ============================================================
 * Forward declarations
 * ============================================================ */
static void prvCSPInsertBlockIntoFreeList( BlockLink_t * pxBlockToInsert );
static void prvCSPHeapInit( void );

/* ============================================================
 * pvCSPMalloc
 * ============================================================ */
void * pvCSPMalloc( size_t xWantedSize )
{
    BlockLink_t * pxBlock;
    BlockLink_t * pxPreviousBlock;
    BlockLink_t * pxNewBlockLink;
    void        * pvReturn = NULL;

    /* (header + alignment) */
    if( xWantedSize > 0 )
    {
        xWantedSize += xCSPHeapStructSize;

        if( ( xWantedSize & CSP_BYTE_ALIGNMENT_MASK ) != 0x00 )
        {
            size_t xExtra = CSP_BYTE_ALIGNMENT - ( xWantedSize & CSP_BYTE_ALIGNMENT_MASK );
            xWantedSize += xExtra;
        }
    }

    vTaskSuspendAll();
    {

        if( pxCSPEnd == NULL )
        {
            prvCSPHeapInit();
        }

        if( cspBLOCK_SIZE_IS_VALID( xWantedSize ) &&
            ( xWantedSize > 0 )                    &&
            ( xWantedSize <= xCSPFreeBytes ) )
        {
            pxPreviousBlock = &xCSPStart;
            pxBlock         = xCSPStart.pxNextFreeBlock;

            while( ( pxBlock->xBlockSize < xWantedSize ) &&
                   ( pxBlock->pxNextFreeBlock != NULL ) )
            {
                pxPreviousBlock = pxBlock;
                pxBlock         = pxBlock->pxNextFreeBlock;
            }

            if( pxBlock != pxCSPEnd )
            {
                pvReturn = ( void * )( ( ( uint8_t * ) pxPreviousBlock->pxNextFreeBlock )
                                       + xCSPHeapStructSize );

                pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

                if( ( pxBlock->xBlockSize - xWantedSize ) > CSP_MINIMUM_BLOCK_SIZE )
                {
                    pxNewBlockLink = ( void * )( ( ( uint8_t * ) pxBlock ) + xWantedSize );
                    pxNewBlockLink->xBlockSize    = pxBlock->xBlockSize - xWantedSize;
                    pxBlock->xBlockSize           = xWantedSize;

                    prvCSPInsertBlockIntoFreeList( pxNewBlockLink );
                }

                xCSPFreeBytes -= pxBlock->xBlockSize;

                if( xCSPFreeBytes < xCSPMinFreeBytes )
                {
                    xCSPMinFreeBytes = xCSPFreeBytes;
                }

                cspALLOCATE_BLOCK( pxBlock );
                pxBlock->pxNextFreeBlock = NULL;
            }
        }
    }
    ( void ) xTaskResumeAll();

    return pvReturn;
}

/* ============================================================
 * vCSPFree
 * ============================================================ */
void vCSPFree( void * pv )
{
    uint8_t     * puc;
    BlockLink_t * pxLink;

    if( pv == NULL )
    {
        return;
    }

    puc    = ( uint8_t * ) pv;
    puc   -= xCSPHeapStructSize;
    pxLink = ( void * ) puc;

    if( !cspBLOCK_IS_ALLOCATED( pxLink ) )
    {
        return;
    }

    if( pxLink->pxNextFreeBlock != NULL )
    {
        return;
    }

    cspFREE_BLOCK( pxLink );

    vTaskSuspendAll();
    {
        xCSPFreeBytes += pxLink->xBlockSize;
        prvCSPInsertBlockIntoFreeList( pxLink );
    }
    ( void ) xTaskResumeAll();
}

size_t xCSPGetFreeHeapSize( void )
{
    return xCSPFreeBytes;
}

size_t xCSPGetMinimumEverFreeHeapSize( void )
{
    return xCSPMinFreeBytes;
}

static void prvCSPHeapInit( void )
{
    BlockLink_t * pxFirstFreeBlock;
    uint8_t     * pucAlignedHeap;
    size_t        xTotalSize = CSP_HEAP_TOTAL_SIZE;
    size_t        xAddress;

    xAddress = ( size_t ) ucCSPHeap;

    if( ( xAddress & CSP_BYTE_ALIGNMENT_MASK ) != 0 )
    {
        xAddress  += ( CSP_BYTE_ALIGNMENT - 1 );
        xAddress  &= ~( ( size_t ) CSP_BYTE_ALIGNMENT_MASK );
        xTotalSize -= ( size_t )( xAddress - ( size_t ) ucCSPHeap );
    }

    pucAlignedHeap = ( uint8_t * ) xAddress;

    xCSPStart.pxNextFreeBlock = ( BlockLink_t * ) pucAlignedHeap;
    xCSPStart.xBlockSize      = ( size_t ) 0;

    xAddress  = ( size_t )( pucAlignedHeap + xTotalSize );
    xAddress -= xCSPHeapStructSize;
    xAddress &= ~( ( size_t ) CSP_BYTE_ALIGNMENT_MASK );
    pxCSPEnd  = ( BlockLink_t * ) xAddress;
    pxCSPEnd->xBlockSize      = 0;
    pxCSPEnd->pxNextFreeBlock = NULL;

    pxFirstFreeBlock = ( BlockLink_t * ) pucAlignedHeap;
    pxFirstFreeBlock->xBlockSize      = ( size_t )( xAddress - ( size_t ) pxFirstFreeBlock );
    pxFirstFreeBlock->pxNextFreeBlock = pxCSPEnd;

    xCSPMinFreeBytes = pxFirstFreeBlock->xBlockSize;
    xCSPFreeBytes    = pxFirstFreeBlock->xBlockSize;
}


static void prvCSPInsertBlockIntoFreeList( BlockLink_t * pxBlockToInsert )
{
    BlockLink_t * pxIterator;
    uint8_t     * puc;

    for( pxIterator = &xCSPStart;
         pxIterator->pxNextFreeBlock < pxBlockToInsert;
         pxIterator = pxIterator->pxNextFreeBlock )
    {
        // Do nothing
    }

    puc = ( uint8_t * ) pxBlockToInsert;
    if( ( puc + pxBlockToInsert->xBlockSize ) == ( uint8_t * ) pxIterator->pxNextFreeBlock )
    {
        if( pxIterator->pxNextFreeBlock != pxCSPEnd )
        {
            pxBlockToInsert->xBlockSize    += pxIterator->pxNextFreeBlock->xBlockSize;
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
        }
        else
        {
            pxBlockToInsert->pxNextFreeBlock = pxCSPEnd;
        }
    }
    else
    {
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }

    puc = ( uint8_t * ) pxIterator;
    if( ( puc + pxIterator->xBlockSize ) == ( uint8_t * ) pxBlockToInsert )
    {
        pxIterator->xBlockSize    += pxBlockToInsert->xBlockSize;
        pxIterator->pxNextFreeBlock = pxBlockToInsert->pxNextFreeBlock;
    }
    else
    {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;
    }
}