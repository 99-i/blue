#pragma once
#include "types.h"
#include "util/bytearray.h"
#include <stdbool.h>

/* Read a varint into <varint>.
 * */
read_result read_varint(bytearray *data, size_t offset, int32_t *varint,
						uint32_t *bytes_read);

/* Read a string into <string>.
 * */
read_result read_string(bytearray *data, size_t offset, string *str,
						uint32_t *bytes_read);

/* Read a bool into <bool>.
 * */
read_result read_bool(bytearray *data, size_t offset, bool *b,
					  uint32_t *bytes_read);

/* Read a int8_t into <i8>.
 * */
read_result read_i8(bytearray *data, size_t offset, int8_t *i8,
					uint32_t *bytes_read);

/* Read a uint8_t into <u8>.
 * */
read_result read_u8(bytearray *data, size_t offset, uint8_t *u8,
					uint32_t *bytes_read);

/* Read a int16_t into <i16>.
 * */
read_result read_i16(bytearray *data, size_t offset, int16_t *i16,
					 uint32_t *bytes_read);

/* Read a uint16_t into <u16>.
 * */
read_result read_u16(bytearray *data, size_t offset, uint16_t *u16,
					 uint32_t *bytes_read);

/* Read a int32_t into <i32>.
 * */
read_result read_i32(bytearray *data, size_t offset, int32_t *i32,
					 uint32_t *bytes_read);

/* Read a int64_t into <i64>.
 * */
read_result read_i64(bytearray *data, size_t offset, int64_t *i64,
					 uint32_t *bytes_read);

/* Read a float into <f>.
 * */
read_result read_float(bytearray *data, size_t offset, float *f,
					   uint32_t *bytes_read);
/* Read a double into <d>.
 * */
read_result read_double(bytearray *data, size_t offset, double *d,
						uint32_t *bytes_read);

/* Read a 64-bit position into <position>.
 * */
read_result read_position(bytearray *data, size_t offset, ipos *position,
						  uint32_t *bytes_read);

/* Read a byte array into <buf>.
 * */
read_result read_bytearray(bytearray *data, size_t offset, bytearray *buf,
						   size_t size, uint32_t *bytes_read);

/* Read a chat_obj into <chat>.
 * */
read_result read_chat(bytearray *data, size_t offset, chat_obj *chat,
					  uint32_t *bytes_read);

/* Read a uuid into <uuid>.
 * */
read_result read_uuid(bytearray *data, size_t offset, uuid *uuid,
					  uint32_t *bytes_read);
