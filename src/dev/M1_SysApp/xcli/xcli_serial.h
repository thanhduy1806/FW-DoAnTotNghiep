#ifndef XCLI_SERIAL_H
#define XCLI_SERIAL_H
/*
 * xcli_serial.h - xCLI named-field binary serialisation.
 *
 * Each field on the wire:
 *   [name_len(1B)][name(N bytes)][type(1B)][data_len(2B LE)][data(N bytes)]
 *
 * Self-describing: the host can decode any response without a schema.
 * All multi-byte integers are little-endian.
 * Strings are UTF-8, NOT null-terminated on the wire.
 *
 * Write example:
 * @code
 *   uint8_t    buf[XCLI_MAX_RESP_TOTAL];
 *   xCLI_Ser_t ser;
 *   xCLI_Ser_Init(&ser, buf, sizeof(buf));
 *   xCLI_Ser_PutU8 (&ser, "pin",   13U);
 *   xCLI_Ser_PutU8 (&ser, "state", 1U);
 *   uint16_t len = xCLI_Ser_Finish(&ser);
 * @endcode
 *
 * Read example:
 * @code
 *   xCLI_Des_t   des;
 *   xCLI_Field_t field;
 *   xCLI_Des_Init(&des, buf, len);
 *   while (xCLI_Des_Next(&des, &field)) {
 *       if (strcmp(field.name, "pin") == 0)
 *           pin = xCLI_Field_U8(&field);
 *   }
 * @endcode
 *                                                  C.H
 */

#include <stdint.h>
#include "xcli_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Field type codes
 * =========================================================================*/
#define XCLI_TYPE_UINT8     0x01U
#define XCLI_TYPE_UINT16    0x02U
#define XCLI_TYPE_UINT32    0x03U
#define XCLI_TYPE_INT8      0x04U
#define XCLI_TYPE_INT16     0x05U
#define XCLI_TYPE_INT32     0x06U
#define XCLI_TYPE_FLOAT     0x07U
#define XCLI_TYPE_BOOL      0x08U
#define XCLI_TYPE_STRING    0x09U
#define XCLI_TYPE_BYTES     0x0AU

/* =========================================================================
 * Serialiser
 * =========================================================================*/
typedef struct {
    uint8_t *buf;
    uint16_t cap;
    uint16_t pos;
    uint8_t  error;
} xCLI_Ser_t;

void     xCLI_Ser_Init    (xCLI_Ser_t *ser, uint8_t *buf, uint16_t cap);
void     xCLI_Ser_PutU8   (xCLI_Ser_t *ser, const char *name, uint8_t  val);
void     xCLI_Ser_PutU16  (xCLI_Ser_t *ser, const char *name, uint16_t val);
void     xCLI_Ser_PutU32  (xCLI_Ser_t *ser, const char *name, uint32_t val);
void     xCLI_Ser_PutI8   (xCLI_Ser_t *ser, const char *name, int8_t   val);
void     xCLI_Ser_PutI16  (xCLI_Ser_t *ser, const char *name, int16_t  val);
void     xCLI_Ser_PutI32  (xCLI_Ser_t *ser, const char *name, int32_t  val);
void     xCLI_Ser_PutFloat(xCLI_Ser_t *ser, const char *name, float    val);
void     xCLI_Ser_PutBool (xCLI_Ser_t *ser, const char *name, uint8_t  val);
void     xCLI_Ser_PutStr  (xCLI_Ser_t *ser, const char *name, const char    *val);
void     xCLI_Ser_PutBytes(xCLI_Ser_t *ser, const char *name, const uint8_t *data, uint16_t len);
uint16_t xCLI_Ser_Finish  (xCLI_Ser_t *ser);
uint8_t  xCLI_Ser_Error   (const xCLI_Ser_t *ser);

/* =========================================================================
 * Deserialiser
 * =========================================================================*/
typedef struct {
    char     name[XCLI_MAX_FIELD_NAME + 1U];
    uint8_t  type;
    uint8_t  data[XCLI_MAX_FIELD_DATA];
    uint16_t data_len;
} xCLI_Field_t;

typedef struct {
    const uint8_t *buf;
    uint16_t       len;
    uint16_t       pos;
    uint8_t        error;
} xCLI_Des_t;

void    xCLI_Des_Init (xCLI_Des_t *des, const uint8_t *buf, uint16_t len);
uint8_t xCLI_Des_Next (xCLI_Des_t *des, xCLI_Field_t *field);
uint8_t xCLI_Des_Error(const xCLI_Des_t *des);

/* =========================================================================
 * Typed accessors
 * =========================================================================*/
uint8_t  xCLI_Field_U8   (const xCLI_Field_t *f);
uint16_t xCLI_Field_U16  (const xCLI_Field_t *f);
uint32_t xCLI_Field_U32  (const xCLI_Field_t *f);
int8_t   xCLI_Field_I8   (const xCLI_Field_t *f);
int16_t  xCLI_Field_I16  (const xCLI_Field_t *f);
int32_t  xCLI_Field_I32  (const xCLI_Field_t *f);
float    xCLI_Field_Float(const xCLI_Field_t *f);
uint8_t  xCLI_Field_Bool (const xCLI_Field_t *f);
/* STRING / BYTES: use f->data and f->data_len directly */

#ifdef __cplusplus
}
#endif

#endif /* XCLI_SERIAL_H */
