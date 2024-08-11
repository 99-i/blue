#include "util/bytearray.h"
#include "mem.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

void bytearray_init(bytearray *array)
{
	array->capacity = 32;
	array->size = 0;
	array->data = blue_malloc(array->capacity * sizeof(uint8_t));
}

void bytearray_append_data(bytearray *array, const uint8_t *data, size_t size)
{
	size_t newsize = array->size + size;
	size_t new_capacity;
	if (newsize > array->capacity)
	{
		new_capacity = array->capacity == 0 ? 1 : array->capacity;

		while (new_capacity < newsize)
		{
			new_capacity = new_capacity * 2;
		}

		array->data = blue_realloc(array->data, (new_capacity) * sizeof(uint8_t));
		array->capacity = new_capacity;
	}

	memcpy(&array->data[array->size], data, size);
	array->size = newsize;
}

void bytearray_slice_from_beginning(bytearray *array, size_t num_elements)
{
	size_t newsize = array->size - num_elements;

	if (num_elements == 0)
		return;

	assert(array->size >= num_elements);

	memmove(&array->data[0], &array->data[num_elements], newsize);
	array->size = newsize;
}

void bytearray_append_byte(bytearray *array, uint8_t byte)
{
	size_t newsize = array->size + 1;
	size_t new_capacity;
	if (newsize > array->capacity)
	{
		new_capacity = array->capacity == 0 ? 1 : array->capacity;

		while (new_capacity < newsize)
		{
			new_capacity = new_capacity * 2;
		}

		array->data = blue_realloc(array->data, (new_capacity) * sizeof(uint8_t));
		array->capacity = new_capacity;
	}

	array->data[array->size] = byte;

	array->size = newsize;
}

void bytearray_insert_byte(bytearray *array, size_t position, uint8_t byte)
{
	size_t newsize = array->size + 1;
	size_t new_capacity;

	assert(position >= 0);
	if (newsize > array->capacity)
	{
		new_capacity = array->capacity == 0 ? 1 : array->capacity;

		while (new_capacity < newsize)
		{
			new_capacity = new_capacity * 2;
		}

		array->data = blue_realloc(array->data, (new_capacity) * sizeof(uint8_t));
		array->capacity = new_capacity;
	}

	if (position < array->size)
	{
		memmove(array->data + position + 1, array->data + position, array->size - position);
	}

	array->data[position] = byte;
	array->size = newsize;
}

void bytearray_print(bytearray *array)
{
	size_t i;
	printf("[");
	for (i = 0; i < array->size; i++)
	{
		printf("%x, ", array->data[i]);
	}
	printf("]");
}
