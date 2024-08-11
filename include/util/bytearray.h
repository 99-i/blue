#pragma once
#include <uv.h>

#include <stdint.h>
typedef struct
{
	uint8_t *data;
	size_t size;
	size_t capacity;
} bytearray;

void bytearray_init(bytearray *array);

#define bytearray_append_uvbuf(array, buf) \
	bytearray_append_data(array, (uint8_t *)buf->base, buf->len)

void bytearray_append_data(bytearray *array, const uint8_t *data, size_t size);

/* in-place slice from index 0 to index <num_elements> exclusive.
 * for example
 * [0, 1, 2, 3, 4].bsfb(3) == [3, 4]
 * [0, 1, 2, 3, 4].bsfb(1) == [1, 2, 3, 4]
 * [0, 1, 2, 3, 4].bsfb(5) == [4]
 * */
void bytearray_slice_from_beginning(bytearray *array, size_t num_elements);

void bytearray_append_byte(bytearray *array, uint8_t byte);

/* insert a byte such that array->data[position] == byte. */
/* handles position 0 and position <size> correctly.*/
void bytearray_insert_byte(bytearray *array, size_t position, uint8_t byte);

/* for debug only. */
void bytearray_print(bytearray *array);
