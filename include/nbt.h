#pragma once
#include "types.h"
#include "util/bytearray.h"
#include "util/string.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	TAG_BYTE = 1,
	TAG_SHORT,
	TAG_INT,
	TAG_LONG,
	TAG_FLOAT,
	TAG_DOUBLE,
	TAG_BYTE_ARRAY,
	TAG_STRING,
	TAG_LIST,
	TAG_COMPOUND,
	TAG_INT_ARRAY,
	TAG_LONG_ARRAY
} nbt_tag_type;

const char *tag_type_to_string(nbt_tag_type type);

typedef struct s_nbt_tag
{
	nbt_tag_type type;
	string name;

	/* if true, either byte array, list, compound, int array, or long array must be deallocated.
	 * if list or compound, must free the children as well. */
	bool allocated;
	union
	{
		int8_t tag_byte;
		int16_t tag_short;
		int32_t tag_int;
		int64_t tag_long;
		float tag_float;
		double tag_double;
		bytearray tag_byte_array;
		string tag_string;
		struct
		{
			nbt_tag_type type;
			size_t size;
			struct s_nbt_tag **tags;
		} tag_list;
		struct
		{
			size_t size;
			struct s_nbt_tag **tags;
		} tag_compound;
		struct
		{
			size_t size;
			int32_t *ints;
		} tag_int_array;
		struct
		{
			size_t size;
			int64_t *longs;
		} tag_long_array;
	};
} nbt_tag;

read_result nbt_read(nbt_tag **tag, bytearray *data, size_t offset);

void nbt_free(nbt_tag *tag);

void nbt_dump(nbt_tag *tag, uint32_t indent);
