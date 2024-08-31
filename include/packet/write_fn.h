#pragma once
#include "types.h"
#include "util/bytearray.h"
#include <stdbool.h>
#include <uuid/uuid.h>

read_result write_varint(bytearray *data, size_t position, int32_t varint, size_t *bytes_written);
read_result write_string(bytearray *data, size_t position, const char *str, size_t *bytes_written);
read_result write_bool(bytearray *data, size_t position, bool b, size_t *bytes_written);
read_result write_i8(bytearray *data, size_t position, int8_t i8, size_t *bytes_written);
read_result write_u8(bytearray *data, size_t position, uint8_t u8, size_t *bytes_written);
read_result write_i16(bytearray *data, size_t position, int16_t i16, size_t *bytes_written);
read_result write_u16(bytearray *data, size_t position, uint16_t u16, size_t *bytes_written);
read_result write_i32(bytearray *data, size_t position, int32_t i32, size_t *bytes_written);
read_result write_i64(bytearray *data, size_t position, int64_t i64, size_t *bytes_written);
read_result write_u64(bytearray *data, size_t position, uint32_t u64, size_t *bytes_written);
read_result write_float(bytearray *data, size_t position, float f, size_t *bytes_written);
read_result write_double(bytearray *data, size_t position, double d, size_t *bytes_written);
read_result write_position(bytearray *data, size_t position, ipos *pos, size_t *bytes_written);
read_result write_bytearray(bytearray *data, size_t position, bytearray *to_write, size_t *bytes_written);
read_result write_uuid(bytearray *data, size_t position, uuid_t uuid, size_t *bytes_written);
