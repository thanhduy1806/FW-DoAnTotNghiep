#include "spi_io.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "define.h"
uint32_t spi_io_set_mode(spi_io_t *me, spi_mode_t mode){
    (void)me; (void)mode;
    return ERROR_OK;
}

/* Synchronous (blocking) */
uint32_t spi_io_read_sync(spi_io_t *me, uint8_t *pui8RxBuff, uint32_t ui32Length)
{
    (void)me; (void)pui8RxBuff; (void)ui32Length;
    return ERROR_OK;
}
uint32_t spi_io_write_sync(spi_io_t *me, const uint8_t *pui8TxBuff, uint32_t ui32Length){
    (void)me; (void)pui8TxBuff; (void)ui32Length;
    return ERROR_OK;
}
uint32_t spi_io_transfer_sync(spi_io_t *me, const uint8_t *pui8TxBuff, uint8_t *pui8RxBuff, uint32_t ui32Length){
    (void)me; (void)pui8TxBuff; (void)pui8RxBuff; (void)ui32Length;
    return ERROR_OK;
}
uint32_t spi_io_write_and_read_sync(spi_io_t *me,
                                    const uint8_t *pui8TxBuff, uint32_t ui32TxLength,
                                    uint8_t *pui8RxBuff, uint32_t ui32RxLength){
    (void)me; (void)pui8TxBuff; (void)ui32TxLength; (void)pui8RxBuff; (void)ui32RxLength;
    return ERROR_OK;
}