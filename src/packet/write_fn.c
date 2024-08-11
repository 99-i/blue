#include "packet/write_fn.h"
#include "types.h"
#include "util/bytearray.h"
#include <assert.h>

#define VARINT_SEGMENT_BITS 0x7F
#define VARINT_CONTINUE_BIT 0x80

/* defining these macros twice... */

read_result write_varint(bytearray *data, size_t position, int32_t varint, size_t *bytes_written)
{
	size_t i = 0;
	while (true)
	{
		if (i == 5)
		{
			assert(false && "unreachable");
			break;
		}
		i++;

		if ((varint & ~VARINT_SEGMENT_BITS) == 0)
		{
			bytearray_insert_byte(data, position++, varint);
			break;
		}

		bytearray_insert_byte(data, position++, (varint & VARINT_SEGMENT_BITS) | VARINT_CONTINUE_BIT);

		varint >>= 7;
	}

	if (bytes_written)
		*bytes_written = i;

	return READ_RESULT_SUCCESS;
}

read_result write_string(bytearray *data, size_t position, string *str, size_t *bytes_written)
{
	size_t i;
	size_t j;
	read_result result;

	result = write_varint(data, position, str->size, &i);

	if (result)
		return result;

	for (j = 0; j < str->size; j++)
	{
		bytearray_insert_byte(data, position + i++, str->data[j]);
	}

	if (bytes_written)
		*bytes_written = i;

	return READ_RESULT_SUCCESS;
}

read_result write_bool(bytearray *data, size_t position, bool b, size_t *bytes_written)
{
	bytearray_insert_byte(data, position, b);
	if (bytes_written)
		*bytes_written = 1;
	return READ_RESULT_SUCCESS;
}

read_result write_i8(bytearray *data, size_t position, int8_t i8, size_t *bytes_written)
{
	bytearray_insert_byte(data, position, i8);
	if (bytes_written)
		*bytes_written = 1;
	return READ_RESULT_SUCCESS;
}

read_result write_u8(bytearray *data, size_t position, uint8_t u8, size_t *bytes_written)
{
	bytearray_insert_byte(data, position, u8);
	if (bytes_written)
		*bytes_written = 1;
	return READ_RESULT_SUCCESS;
}

read_result write_i16(bytearray *data, size_t position, int16_t i16, size_t *bytes_written)
{
	bytearray_insert_byte(data, position++, (i16 & 0xff00) >> 8);
	bytearray_insert_byte(data, position++, (i16 & 0xff));
	if (bytes_written)
		*bytes_written = 2;
	return READ_RESULT_SUCCESS;
}

read_result write_u16(bytearray *data, size_t position, uint16_t u16, size_t *bytes_written)
{
	bytearray_insert_byte(data, position++, (u16 & 0xff00) >> 8);
	bytearray_insert_byte(data, position++, (u16 & 0xff));
	if (bytes_written)
		*bytes_written = 2;
	return READ_RESULT_SUCCESS;
}

read_result write_i32(bytearray *data, size_t position, int32_t i32, size_t *bytes_written)
{
	bytearray_insert_byte(data, position++, (i32 & 0xff000000) >> 24);
	bytearray_insert_byte(data, position++, (i32 & 0xff0000) >> 16);
	bytearray_insert_byte(data, position++, (i32 & 0xff00) >> 8);
	bytearray_insert_byte(data, position++, (i32 & 0xff));
	if (bytes_written)
		*bytes_written = 4;
	return READ_RESULT_SUCCESS;
}

read_result write_i64(bytearray *data, size_t position, int64_t i64, size_t *bytes_written)
{
	bytearray_insert_byte(data, position++, (i64 & 0xff00000000000000) >> 56);
	bytearray_insert_byte(data, position++, (i64 & 0xff000000000000) >> 48);
	bytearray_insert_byte(data, position++, (i64 & 0xff0000000000) >> 40);
	bytearray_insert_byte(data, position++, (i64 & 0xff00000000) >> 32);
	bytearray_insert_byte(data, position++, (i64 & 0xff000000) >> 24);
	bytearray_insert_byte(data, position++, (i64 & 0xff0000) >> 16);
	bytearray_insert_byte(data, position++, (i64 & 0xff00) >> 8);
	bytearray_insert_byte(data, position++, (i64 & 0xff));
	if (bytes_written)
		*bytes_written = 8;
	return READ_RESULT_SUCCESS;
}

read_result write_u64(bytearray *data, size_t position, uint32_t u64, size_t *bytes_written)
{
	bytearray_insert_byte(data, position++, (u64 & 0xff00000000000000) >> 56);
	bytearray_insert_byte(data, position++, (u64 & 0xff000000000000) >> 48);
	bytearray_insert_byte(data, position++, (u64 & 0xff0000000000) >> 40);
	bytearray_insert_byte(data, position++, (u64 & 0xff00000000) >> 32);
	bytearray_insert_byte(data, position++, (u64 & 0xff000000) >> 24);
	bytearray_insert_byte(data, position++, (u64 & 0xff0000) >> 16);
	bytearray_insert_byte(data, position++, (u64 & 0xff00) >> 8);
	bytearray_insert_byte(data, position++, (u64 & 0xff));
	if (bytes_written)
		*bytes_written = 8;
	return READ_RESULT_SUCCESS;
}

read_result write_float(bytearray *data, size_t position, float f, size_t *bytes_written)
{
	bytearray_insert_byte(data, position++, (((int32_t) * ((int32_t *)&f)) & 0xff000000) >> 24);
	bytearray_insert_byte(data, position++, (((int32_t) * ((int32_t *)&f)) & 0xff0000) >> 16);
	bytearray_insert_byte(data, position++, (((int32_t) * ((int32_t *)&f)) & 0xff00) >> 8);
	bytearray_insert_byte(data, position++, (((int32_t) * ((int32_t *)&f)) & 0xff));
	if (bytes_written)
		*bytes_written = 4;
	return READ_RESULT_SUCCESS;
}

read_result write_double(bytearray *data, size_t position, double d, size_t *bytes_written)
{
	bytearray_insert_byte(data, position++, (((int64_t) * ((int64_t *)&d)) & 0xff00000000000000) >> 56);
	bytearray_insert_byte(data, position++, (((int64_t) * ((int64_t *)&d)) & 0xff000000000000) >> 48);
	bytearray_insert_byte(data, position++, (((int64_t) * ((int64_t *)&d)) & 0xff0000000000) >> 40);
	bytearray_insert_byte(data, position++, (((int64_t) * ((int64_t *)&d)) & 0xff00000000) >> 32);
	bytearray_insert_byte(data, position++, (((int64_t) * ((int64_t *)&d)) & 0xff000000) >> 24);
	bytearray_insert_byte(data, position++, (((int64_t) * ((int64_t *)&d)) & 0xff0000) >> 16);
	bytearray_insert_byte(data, position++, (((int64_t) * ((int64_t *)&d)) & 0xff00) >> 8);
	bytearray_insert_byte(data, position++, (((int64_t) * ((int64_t *)&d)) & 0xff));
	if (bytes_written)
		*bytes_written = 8;
	return READ_RESULT_SUCCESS;
}

read_result write_position(bytearray *data, size_t position, ipos *pos, size_t *bytes_written)
{
	uint64_t i = ((((int64_t)pos->x) & 0x3FFFFFF) << 38) | ((((int64_t)pos->z) & 0x3FFFFFF) << 12) | (pos->y & 0xFFF);

	return write_u64(data, position, i, bytes_written);
}

read_result write_bytearray(bytearray *data, size_t position, bytearray *to_write, size_t *bytes_written)
{
	size_t i;
	for (i = 0; i < to_write->size; i++)
	{
		bytearray_insert_byte(data, position++, to_write->data[i]);
	}
	if (bytes_written)
		*bytes_written = to_write->size;
	return READ_RESULT_SUCCESS;
}

read_result write_uuid(bytearray *data, size_t position, uuid id, size_t *bytes_written)
{
	read_result result;

	result = write_i64(data, position, id[0], NULL);
	if (result)
		return result;
	result = write_i64(data, position, id[1], NULL);
	if (result)
		return result;

	if (bytes_written)
		*bytes_written = 16;

	return READ_RESULT_SUCCESS;
}
