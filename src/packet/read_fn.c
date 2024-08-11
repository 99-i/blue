#include "packet/read_fn.h"
#include "mem.h"
#include <string.h>
#define VARINT_SEGMENT_BITS 0x7F
#define VARINT_CONTINUE_BIT 0x80

read_result read_varint(bytearray *data, size_t offset, int32_t *varint, uint32_t *bytes_read)
{
	int value = 0;
	int position = 0;
	uint32_t bytes_read_counter = 0;
	uint8_t byte;

	while (1)
	{
		if (offset >= data->size)
		{
			return READ_RESULT_NOT_ENOUGH_DATA;
		}

		byte = data->data[offset++];
		bytes_read_counter++;
		value |= (byte & VARINT_SEGMENT_BITS) << position;

		if ((byte & VARINT_CONTINUE_BIT) == 0)
			break;

		position += 7;

		if (position >= 32)
		{
			return READ_RESULT_MALFORMED;
		}
	}

	*varint = value;
	*bytes_read = bytes_read_counter;
	return READ_RESULT_SUCCESS;
}

read_result read_string(bytearray *data, size_t offset, string *str, uint32_t *bytes_read)
{
	uint32_t length_size;
	int32_t length;
	read_result result;

	result = read_varint(data, offset, &length, &length_size);
	if (result)
		return result;

	offset += length_size;

	if (offset + length >= data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;

	str->data = blue_malloc(length * sizeof(uint8_t));
	str->size = length;

	memcpy(str->data, &data->data[offset], length);
	*bytes_read = length_size + length;
	return READ_RESULT_SUCCESS;
}

read_result read_bool(bytearray *data, size_t offset, bool *b,
					  uint32_t *bytes_read)
{
	if (offset == data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;
	*b = data->data[offset];
	*bytes_read = 1;
	return READ_RESULT_SUCCESS;
}

read_result read_i8(bytearray *data, size_t offset, int8_t *i8, uint32_t *bytes_read)
{
	if (offset == data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;
	*i8 = data->data[offset];
	*bytes_read = 1;
	return READ_RESULT_SUCCESS;
}

read_result read_u8(bytearray *data, size_t offset, uint8_t *u8, uint32_t *bytes_read)
{
	if (offset >= data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;
	*u8 = data->data[offset];
	*bytes_read = 1;
	return READ_RESULT_SUCCESS;
}

read_result read_i16(bytearray *data, size_t offset, int16_t *i16, uint32_t *bytes_read)
{
	if (offset + 1 >= data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;
	*i16 = (data->data[offset + 1] << 8) | data->data[offset];
	*bytes_read = 2;
	return READ_RESULT_SUCCESS;
}

read_result read_u16(bytearray *data, size_t offset, uint16_t *u16, uint32_t *bytes_read)
{
	if (offset + 1 >= data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;
	*u16 = (data->data[offset + 1] << 8) | data->data[offset];
	*bytes_read = 2;
	return READ_RESULT_SUCCESS;
}

read_result read_i32(bytearray *data, size_t offset, int32_t *i32, uint32_t *bytes_read)
{
	if (offset + 3 >= data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;
	*i32 = (data->data[offset + 3] << 24) | (data->data[offset + 2] << 16) |
		   (data->data[offset + 1] << 8) | data->data[offset];
	*bytes_read = 4;
	return READ_RESULT_SUCCESS;
}

read_result read_i64(bytearray *data, size_t offset, int64_t *i64, uint32_t *bytes_read)
{
	if (offset + 7 >= data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;
	*i64 = (data->data[offset + 7] << 24) | (data->data[offset + 6] << 16) |
		   (data->data[offset + 5] << 24) | (data->data[offset + 4] << 16) |
		   (data->data[offset + 3] << 24) | (data->data[offset + 2] << 16) |
		   (data->data[offset + 1] << 8) | data->data[offset];
	*bytes_read = 8;
	return READ_RESULT_SUCCESS;
}

read_result read_float(bytearray *data, size_t offset, float *f, uint32_t *bytes_read)
{
	int32_t i;
	if (offset + 3 >= data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;
	i = (data->data[offset + 3] << 24) | (data->data[offset + 2] << 16) |
		(data->data[offset + 1] << 8) | data->data[offset];
	*((uint32_t *)&f) = i;

	*bytes_read = 4;
	return READ_RESULT_SUCCESS;
}

read_result read_double(bytearray *data, size_t offset, double *d, uint32_t *bytes_read)
{
	int64_t i;
	if (offset + 7 >= data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;
	i = (data->data[offset + 7] << 24) | (data->data[offset + 6] << 16) |
		(data->data[offset + 5] << 24) | (data->data[offset + 4] << 16) |
		(data->data[offset + 3] << 24) | (data->data[offset + 2] << 16) |
		(data->data[offset + 1] << 8) | data->data[offset];
	*((uint64_t *)&d) = i;

	*bytes_read = 8;
	return READ_RESULT_SUCCESS;
}

read_result read_bytearray(bytearray *data, size_t offset, bytearray *buf, size_t size, uint32_t *bytes_read)
{
	if (offset + (size - 1) >= data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;

	buf->data = blue_malloc(size);
	buf->size = size;
	buf->capacity = size;

	memcpy(buf->data, data->data + offset, size);

	*bytes_read = size;
	return READ_RESULT_SUCCESS;
}

read_result read_position(bytearray *data, size_t offset, ipos *position, uint32_t *bytes_read)
{
	int64_t i;
	if (offset + 7 >= data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;
	i = (data->data[offset + 7] << 24) | (data->data[offset + 6] << 16) |
		(data->data[offset + 5] << 24) | (data->data[offset + 4] << 16) |
		(data->data[offset + 3] << 24) | (data->data[offset + 2] << 16) |
		(data->data[offset + 1] << 8) | data->data[offset];

	position->x = i >> 38;
	position->y = i << 52 >> 52;
	position->z = i << 26 >> 38;

	*bytes_read = 8;
	return READ_RESULT_SUCCESS;
}

read_result read_chat(bytearray *data, size_t offset, chat_obj *chat, uint32_t *bytes_read)
{
	read_result result;
	result = read_string(data, offset, &chat->raw_json, bytes_read);
	if (result)
		return result;

	chat->json =
		cJSON_ParseWithLength((char *)chat->raw_json.data, chat->raw_json.size);

	return READ_RESULT_SUCCESS;
}

read_result read_uuid(bytearray *data, size_t offset, uuid *uuid, uint32_t *bytes_read)
{
	if (offset + 14 >= data->size)
		return READ_RESULT_NOT_ENOUGH_DATA;

	*uuid[0] = (data->data[offset + 7] << 24) | (data->data[offset + 6] << 16) |
			   (data->data[offset + 5] << 24) | (data->data[offset + 4] << 16) |
			   (data->data[offset + 3] << 24) | (data->data[offset + 2] << 16) |
			   (data->data[offset + 1] << 8) | data->data[offset];

	*uuid[1] = (data->data[offset + 15] << 24) | (data->data[offset + 14] << 16) |
			   (data->data[offset + 13] << 24) | (data->data[offset + 12] << 16) |
			   (data->data[offset + 11] << 24) | (data->data[offset + 10] << 16) |
			   (data->data[offset + 9] << 8) | data->data[offset + 8];

	*bytes_read = 16;

	return READ_RESULT_SUCCESS;
}
