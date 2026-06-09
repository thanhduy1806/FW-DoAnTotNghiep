/*
 * xcli_serial.c   xCLI named-field binary serialisation.
 *
 * Wire encoding per field:
 *   [name_len(1B)][name(N bytes)][type(1B)][data_len(2B LE)][data(N bytes)]
 *
 * On overflow / underflow: error flag is set; subsequent calls are no-ops.
 * All multi-byte values are little-endian.
 *                                                  C.H
 */

#include "xcli_serial.h"
#include <string.h>

/* =========================================================================
 * Internal helper
 * =========================================================================*/
static void xcli_ser_field(xCLI_Ser_t    *ser,
                            const char    *name,
                            uint8_t        type,
                            const uint8_t *data,
                            uint16_t       data_len)
{
    uint8_t  name_len;
    uint16_t needed;

    if (ser == NULL || ser->error) { return; }
    if (name == NULL)               { ser->error = 1U; return; }

    name_len = 0U;
    while (name[name_len] != '\0' && name_len < 255U) { name_len++; }

    needed = (uint16_t)(1U + (uint16_t)name_len + 1U + 2U + data_len);
    if ((uint16_t)(ser->cap - ser->pos) < needed) { ser->error = 1U; return; }

    ser->buf[ser->pos++] = name_len;
    (void)memcpy(&ser->buf[ser->pos], name, (size_t)name_len);
    ser->pos = (uint16_t)(ser->pos + name_len);

    ser->buf[ser->pos++] = type;
    ser->buf[ser->pos++] = (uint8_t)(data_len & 0xFFU);
    ser->buf[ser->pos++] = (uint8_t)((data_len >> 8U) & 0xFFU);

    if (data != NULL && data_len > 0U) {
        (void)memcpy(&ser->buf[ser->pos], data, (size_t)data_len);
        ser->pos = (uint16_t)(ser->pos + data_len);
    }
}

/* =========================================================================
 * Serialiser
 * =========================================================================*/
void xCLI_Ser_Init(xCLI_Ser_t *ser, uint8_t *buf, uint16_t cap)
{
    if (ser == NULL) { return; }
    ser->buf   = buf;
    ser->cap   = cap;
    ser->pos   = 0U;
    ser->error = 0U;
}

void xCLI_Ser_PutU8(xCLI_Ser_t *ser, const char *name, uint8_t val)
{
    xcli_ser_field(ser, name, XCLI_TYPE_UINT8, &val, 1U);
}

void xCLI_Ser_PutU16(xCLI_Ser_t *ser, const char *name, uint16_t val)
{
    uint8_t raw[2];
    raw[0] = (uint8_t)(val & 0xFFU);
    raw[1] = (uint8_t)((val >> 8U) & 0xFFU);
    xcli_ser_field(ser, name, XCLI_TYPE_UINT16, raw, 2U);
}

void xCLI_Ser_PutU32(xCLI_Ser_t *ser, const char *name, uint32_t val)
{
    uint8_t raw[4];
    raw[0] = (uint8_t)(val & 0xFFU);
    raw[1] = (uint8_t)((val >>  8U) & 0xFFU);
    raw[2] = (uint8_t)((val >> 16U) & 0xFFU);
    raw[3] = (uint8_t)((val >> 24U) & 0xFFU);
    xcli_ser_field(ser, name, XCLI_TYPE_UINT32, raw, 4U);
}

void xCLI_Ser_PutI8(xCLI_Ser_t *ser, const char *name, int8_t val)
{
    uint8_t raw = (uint8_t)val;
    xcli_ser_field(ser, name, XCLI_TYPE_INT8, &raw, 1U);
}

void xCLI_Ser_PutI16(xCLI_Ser_t *ser, const char *name, int16_t val)
{
    uint16_t uval = (uint16_t)val;
    uint8_t  raw[2];
    raw[0] = (uint8_t)(uval & 0xFFU);
    raw[1] = (uint8_t)((uval >> 8U) & 0xFFU);
    xcli_ser_field(ser, name, XCLI_TYPE_INT16, raw, 2U);
}

void xCLI_Ser_PutI32(xCLI_Ser_t *ser, const char *name, int32_t val)
{
    uint32_t uval = (uint32_t)val;
    uint8_t  raw[4];
    raw[0] = (uint8_t)(uval & 0xFFU);
    raw[1] = (uint8_t)((uval >>  8U) & 0xFFU);
    raw[2] = (uint8_t)((uval >> 16U) & 0xFFU);
    raw[3] = (uint8_t)((uval >> 24U) & 0xFFU);
    xcli_ser_field(ser, name, XCLI_TYPE_INT32, raw, 4U);
}

void xCLI_Ser_PutFloat(xCLI_Ser_t *ser, const char *name, float val)
{
    uint8_t raw[4];
    (void)memcpy(raw, &val, 4U);
    xcli_ser_field(ser, name, XCLI_TYPE_FLOAT, raw, 4U);
}

void xCLI_Ser_PutBool(xCLI_Ser_t *ser, const char *name, uint8_t val)
{
    uint8_t raw = (val != 0U) ? 1U : 0U;
    xcli_ser_field(ser, name, XCLI_TYPE_BOOL, &raw, 1U);
}

void xCLI_Ser_PutStr(xCLI_Ser_t *ser, const char *name, const char *val)
{
    uint16_t slen = 0U;
    if (ser == NULL || ser->error) { return; }
    if (val != NULL) {
        while (val[slen] != '\0') { slen++; if (slen == 0xFFFFU) { break; } }
    }
    xcli_ser_field(ser, name, XCLI_TYPE_STRING,
                   (val != NULL) ? (const uint8_t *)val : NULL, slen);
}

void xCLI_Ser_PutBytes(xCLI_Ser_t *ser, const char *name,
                        const uint8_t *data, uint16_t len)
{
    xcli_ser_field(ser, name, XCLI_TYPE_BYTES, data, len);
}

uint16_t xCLI_Ser_Finish(xCLI_Ser_t *ser)
{
    if (ser == NULL || ser->error) { return 0U; }
    return ser->pos;
}

uint8_t xCLI_Ser_Error(const xCLI_Ser_t *ser)
{
    if (ser == NULL) { return 1U; }
    return ser->error;
}

/* =========================================================================
 * Deserialiser
 * =========================================================================*/
void xCLI_Des_Init(xCLI_Des_t *des, const uint8_t *buf, uint16_t len)
{
    if (des == NULL) { return; }
    des->buf   = buf;
    des->len   = len;
    des->pos   = 0U;
    des->error = 0U;
}

uint8_t xCLI_Des_Next(xCLI_Des_t *des, xCLI_Field_t *field)
{
    uint8_t  name_len;
    uint16_t data_len;
    uint16_t copy_len;

    if (des == NULL || field == NULL || des->error) { return 0U; }
    if (des->pos >= des->len)                       { return 0U; }

    if ((uint16_t)(des->len - des->pos) < 1U) { des->error = 1U; return 0U; }
    name_len = des->buf[des->pos++];

    if ((uint16_t)(des->len - des->pos) < (uint16_t)name_len) { des->error = 1U; return 0U; }
    copy_len = (name_len <= XCLI_MAX_FIELD_NAME) ? name_len : XCLI_MAX_FIELD_NAME;
    (void)memcpy(field->name, &des->buf[des->pos], (size_t)copy_len);
    field->name[copy_len] = '\0';
    des->pos = (uint16_t)(des->pos + name_len);

    if ((uint16_t)(des->len - des->pos) < 1U) { des->error = 1U; return 0U; }
    field->type = des->buf[des->pos++];

    if ((uint16_t)(des->len - des->pos) < 2U) { des->error = 1U; return 0U; }
    data_len  = (uint16_t)(des->buf[des->pos]);
    data_len |= (uint16_t)((uint16_t)(des->buf[des->pos + 1U]) << 8U);
    des->pos  = (uint16_t)(des->pos + 2U);

    if ((uint16_t)(des->len - des->pos) < data_len) { des->error = 1U; return 0U; }
    copy_len = (data_len <= XCLI_MAX_FIELD_DATA) ? data_len : XCLI_MAX_FIELD_DATA;
    field->data_len = data_len;
    (void)memcpy(field->data, &des->buf[des->pos], (size_t)copy_len);
    if (copy_len < XCLI_MAX_FIELD_DATA) {
        (void)memset(&field->data[copy_len], 0,
                     (size_t)(XCLI_MAX_FIELD_DATA - copy_len));
    }
    des->pos = (uint16_t)(des->pos + data_len);
    return 1U;
}

uint8_t xCLI_Des_Error(const xCLI_Des_t *des)
{
    if (des == NULL) { return 1U; }
    return des->error;
}

/* =========================================================================
 * Typed accessors
 * =========================================================================*/
uint8_t xCLI_Field_U8(const xCLI_Field_t *f)
{
    if (f == NULL || f->data_len < 1U) { return 0U; }
    return f->data[0];
}

uint16_t xCLI_Field_U16(const xCLI_Field_t *f)
{
    uint16_t v;
    if (f == NULL || f->data_len < 2U) { return 0U; }
    v  = (uint16_t)(f->data[0]);
    v |= (uint16_t)((uint16_t)(f->data[1]) << 8U);
    return v;
}

uint32_t xCLI_Field_U32(const xCLI_Field_t *f)
{
    uint32_t v;
    if (f == NULL || f->data_len < 4U) { return 0U; }
    v  = (uint32_t)(f->data[0]);
    v |= (uint32_t)((uint32_t)(f->data[1]) <<  8U);
    v |= (uint32_t)((uint32_t)(f->data[2]) << 16U);
    v |= (uint32_t)((uint32_t)(f->data[3]) << 24U);
    return v;
}

int8_t xCLI_Field_I8(const xCLI_Field_t *f)
{
    if (f == NULL || f->data_len < 1U) { return 0; }
    return (int8_t)(f->data[0]);
}

int16_t xCLI_Field_I16(const xCLI_Field_t *f)
{
    uint16_t v;
    if (f == NULL || f->data_len < 2U) { return 0; }
    v  = (uint16_t)(f->data[0]);
    v |= (uint16_t)((uint16_t)(f->data[1]) << 8U);
    return (int16_t)v;
}

int32_t xCLI_Field_I32(const xCLI_Field_t *f)
{
    uint32_t v;
    if (f == NULL || f->data_len < 4U) { return 0; }
    v  = (uint32_t)(f->data[0]);
    v |= (uint32_t)((uint32_t)(f->data[1]) <<  8U);
    v |= (uint32_t)((uint32_t)(f->data[2]) << 16U);
    v |= (uint32_t)((uint32_t)(f->data[3]) << 24U);
    return (int32_t)v;
}

float xCLI_Field_Float(const xCLI_Field_t *f)
{
    float v;
    if (f == NULL || f->data_len < 4U) { return 0.0f; }
    (void)memcpy(&v, f->data, 4U);
    return v;
}

uint8_t xCLI_Field_Bool(const xCLI_Field_t *f)
{
    if (f == NULL || f->data_len < 1U) { return 0U; }
    return (f->data[0] != 0U) ? 1U : 0U;
}
