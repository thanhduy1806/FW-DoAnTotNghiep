#ifndef GS_PARAM_TABLE_H
#define GS_PARAM_TABLE_H
/* Copyright (c) 2013-2018 GomSpace A/S. All rights reserved. */

#include <param/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Table instance flags (bit 16-23) */
#define GS_PARAM_TABLE_F_ALLOC_MEMORY  0x0100
#define GS_PARAM_TABLE_F_ALLOC_ROWS    0x0200
#define GS_PARAM_TABLE_F_NO_LOCK       0x0400

/**
   Calculate memory size based on table rows.
*/
size_t gs_param_calc_table_size(const gs_param_table_row_t * rows, size_t row_count);

/**
   Return size of table instance struct.
*/
size_t gs_param_table_instance_size(void);

/**
   Clear a user-allocated table instance buffer.
   Use this instead of gs_param_table_instance_alloc() on MCU (no heap).

   Example:
     static uint8_t tinst_buf[...] __attribute__((aligned(4)));
     gs_param_table_instance_t * tinst = gs_param_table_instance_clear(tinst_buf, sizeof(tinst_buf));
*/
gs_param_table_instance_t * gs_param_table_instance_clear(void * var, size_t var_size);

/**
   Find row by name.
*/
const gs_param_table_row_t * gs_param_row_by_name(const char * name, const gs_param_table_row_t * rows, size_t row_count);

/**
   Find row by address.
*/
const gs_param_table_row_t * gs_param_row_by_address(uint16_t addr, const gs_param_table_row_t * rows, size_t row_count);

/**
   Return table memory pointer.
*/
void * gs_param_table_get_memory(gs_param_table_instance_t * tinst, size_t * return_size);

/**
   Return table rows pointer.
*/
const gs_param_table_row_t * gs_param_table_get_rows(gs_param_table_instance_t * tinst, size_t * return_count);

/**
   Lock table (recursive mutex).
*/
gs_error_t gs_param_table_lock(gs_param_table_instance_t * tinst);

/**
   Unlock table.
*/
gs_error_t gs_param_table_unlock(gs_param_table_instance_t * tinst);

/**
   Free internal resources (mutex) and clear instance.
   Does NOT free tinst itself — caller manages static allocation.
*/
gs_error_t gs_param_table_free(gs_param_table_instance_t * tinst);

/**
   Get/set parameter by address (low-level).
*/
gs_error_t gs_param_get(gs_param_table_instance_t * tinst, uint16_t addr, gs_param_type_t type, void * value, size_t value_size, uint32_t flags);
gs_error_t gs_param_set(gs_param_table_instance_t * tinst, uint16_t addr, gs_param_type_t type, const void * value, size_t value_size, uint32_t flags);

/**
   Convenience wrappers for string and data parameters.
*/
gs_error_t gs_param_get_string(gs_param_table_instance_t * tinst, uint16_t addr, char * buf, size_t buf_size, uint32_t flags);
gs_error_t gs_param_set_string(gs_param_table_instance_t * tinst, uint16_t addr, const char * value, uint32_t flags);
gs_error_t gs_param_get_data(gs_param_table_instance_t * tinst, uint16_t addr, void * buf, size_t buf_size, uint32_t flags);
gs_error_t gs_param_set_data(gs_param_table_instance_t * tinst, uint16_t addr, const void * value, size_t value_size, uint32_t flags);

/**
   Checksum functions.
*/
uint16_t gs_param_table_checksum(gs_param_table_instance_t * tinst);
uint16_t gs_param_table_checksum_be(gs_param_table_instance_t * tinst);
uint16_t gs_param_table_checksum_le(gs_param_table_instance_t * tinst);

/**
   Return size of a parameter type in bytes.
*/
uint8_t gs_param_type_size(gs_param_type_t type);

#ifdef __cplusplus
}
#endif

#endif // GS_PARAM_TABLE_H