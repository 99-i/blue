#include "nbt.h"
#include "mem.h"
#include "packet/read_fn.h"
#include "util/bytearray.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	uint8_t *data;
	int64_t data_size;
	int64_t needle;
} read_state;

/* increment the needle and return the byte at the incremented needle */
static uint8_t next(read_state *state);

/* return the byte at the next needle without incrementing it. */
static uint8_t peek(read_state *state);

/* read an id byte and call the corresponding read function. implicit id_prefixed = true*/
static read_result read_tag(nbt_tag **out_tag, read_state *state, bool has_name);

/* read the nbt tag and set the needle to the last byte of the tag. expects the needle to be at the byte before
 * the start of the data. */

static read_result read_nbt_tag_byte(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed);
static read_result read_nbt_tag_short(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed);
static read_result read_nbt_tag_int(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed);
static read_result read_nbt_tag_long(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed);
static read_result read_nbt_tag_float(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed);
static read_result read_nbt_tag_double(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed);
static read_result read_nbt_tag_byte_array(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed);
static read_result read_nbt_tag_string(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed);
static read_result read_nbt_tag_list(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed);

/* if this compound is suffixed by a 0 byte. the only case when it isn't is when reading the top-level implicit compound */
static read_result read_nbt_tag_compound(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed, bool ending_nul_byte);
static read_result read_nbt_tag_int_array(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed);
static read_result read_nbt_tag_long_array(nbt_tag **out_tag, read_state *state, bool has_name, bool id_prefixed);

/* read a varint, then a string. needle is set to the last byte of the string. */
static read_result read_nbt_string(read_state *state, char **str);

read_result nbt_read(nbt_tag **tag, bytearray *data, size_t offset)
{
	read_state state;
	state.data = data->data + offset;
	state.data_size = data->size - offset;
	state.needle = -1;

	return read_nbt_tag_compound(tag, &state, false, false, false);
}

void nbt_free(nbt_tag *tag)
{
	size_t i;
	if (!tag->allocated)
		return;
	switch (tag->type)
	{
		case TAG_BYTE:
		case TAG_SHORT:
		case TAG_INT:
		case TAG_LONG:
		case TAG_FLOAT:
		case TAG_DOUBLE:
			assert(false && "? Allocated set to true on non-applicable nbt tag.");
			break;
		case TAG_BYTE_ARRAY:
			blue_free(tag->tag_byte_array.data);
			break;
		case TAG_STRING:
			blue_free(tag->tag_string);
			break;
		case TAG_LIST:
			for (i = 0; i < tag->tag_list.size; i++)
			{
				nbt_free(tag->tag_list.tags[i]);
			}
			break;
		case TAG_COMPOUND:
			for (i = 0; i < tag->tag_compound.size; i++)
			{
				nbt_free(tag->tag_compound.tags[i]);
			}
			break;
		case TAG_INT_ARRAY:
			blue_free(tag->tag_int_array.ints);
			break;
		case TAG_LONG_ARRAY:
			blue_free(tag->tag_long_array.longs);
			break;
	}
}

static read_result read_tag(nbt_tag **tag, read_state *state, bool has_name)
{
	if (state->needle + 1 >= state->data_size)
		return READ_RESULT_MALFORMED;

	switch (peek(state))
	{
		case TAG_BYTE:
			{
				return read_nbt_tag_byte(tag, state, has_name, true);
			}
			break;
		case TAG_SHORT:
			{
				return read_nbt_tag_short(tag, state, has_name, true);
			}
			break;
		case TAG_INT:
			{
				return read_nbt_tag_int(tag, state, has_name, true);
			}
			break;
		case TAG_LONG:
			{
				return read_nbt_tag_long(tag, state, has_name, true);
			}
			break;
		case TAG_FLOAT:
			{
				return read_nbt_tag_float(tag, state, has_name, true);
			}
			break;
		case TAG_DOUBLE:
			{
				return read_nbt_tag_double(tag, state, has_name, true);
			}
			break;
		case TAG_BYTE_ARRAY:
			{
				return read_nbt_tag_byte_array(tag, state, has_name, true);
			}
			break;
		case TAG_STRING:
			{
				return read_nbt_tag_string(tag, state, has_name, true);
			}
			break;
		case TAG_LIST:
			{
				return read_nbt_tag_list(tag, state, has_name, true);
			}
			break;
		case TAG_COMPOUND:
			{
				return read_nbt_tag_compound(tag, state, has_name, true, true);
			}
			break;
		case TAG_INT_ARRAY:
			{
				return read_nbt_tag_int_array(tag, state, has_name, true);
			}
			break;
		case TAG_LONG_ARRAY:
			{
				return read_nbt_tag_long_array(tag, state, has_name, true);
			}
			break;
	}
	return READ_RESULT_MALFORMED;
}

#define READ_NAME                                    \
	if (has_name)                                    \
	{                                                \
		if (read_nbt_string(state, &temp_tag->name)) \
		{                                            \
			blue_free(temp_tag);                     \
			return READ_RESULT_MALFORMED;            \
		}                                            \
	}                                                \
	else                                             \
	{                                                \
		temp_tag->name = NULL;                       \
	}

static read_result read_nbt_tag_byte(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));

	if (id_prefixed && next(state) != TAG_BYTE)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_BYTE;

	READ_NAME

	if (state->needle + 1 >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->tag_byte = next(state);
	temp_tag->allocated = false;
	*tag = temp_tag;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_tag_short(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));

	if (id_prefixed && next(state) != TAG_SHORT)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_SHORT;

	READ_NAME

	if (state->needle + 2 >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->tag_short = 0;
	temp_tag->tag_short |= (int16_t)next(state) << 8;
	temp_tag->tag_short |= next(state);

	*tag = temp_tag;
	temp_tag->allocated = false;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_tag_int(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));

	if (id_prefixed && next(state) != TAG_INT)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_INT;

	READ_NAME

	if (state->needle + 4 >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->tag_int = 0;
	temp_tag->tag_int |= (int32_t)next(state) << 24;
	temp_tag->tag_int |= (int32_t)next(state) << 16;
	temp_tag->tag_int |= (int32_t)next(state) << 8;
	temp_tag->tag_int |= next(state);

	*tag = temp_tag;
	temp_tag->allocated = false;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_tag_long(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));

	if (id_prefixed && next(state) != TAG_LONG)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_LONG;

	READ_NAME

	if (state->needle + 8 >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->tag_long = 0;
	temp_tag->tag_long |= (int64_t)next(state) << 56;
	temp_tag->tag_long |= (int64_t)next(state) << 48;
	temp_tag->tag_long |= (int64_t)next(state) << 40;
	temp_tag->tag_long |= (int64_t)next(state) << 32;
	temp_tag->tag_long |= (int64_t)next(state) << 24;
	temp_tag->tag_long |= (int64_t)next(state) << 16;
	temp_tag->tag_long |= (int64_t)next(state) << 8;
	temp_tag->tag_long |= next(state);

	*tag = temp_tag;
	temp_tag->allocated = false;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_tag_float(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));
	int32_t i = 0;

	if (id_prefixed && next(state) != TAG_FLOAT)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_FLOAT;

	READ_NAME

	if (state->needle + 4 >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	i |= (int32_t)next(state) << 24;
	i |= (int32_t)next(state) << 16;
	i |= (int32_t)next(state) << 8;
	i |= next(state);

	*((int32_t *)&temp_tag->tag_float) = i;

	*tag = temp_tag;
	temp_tag->allocated = false;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_tag_double(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));
	int64_t l = 0;

	if (id_prefixed && next(state) != TAG_DOUBLE)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_DOUBLE;

	READ_NAME

	if (state->needle + 8 >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	l |= (int64_t)next(state) << 56;
	l |= (int64_t)next(state) << 48;
	l |= (int64_t)next(state) << 40;
	l |= (int64_t)next(state) << 32;
	l |= (int64_t)next(state) << 24;
	l |= (int64_t)next(state) << 16;
	l |= (int64_t)next(state) << 8;
	l |= next(state);

	*((int64_t *)&temp_tag->tag_double) = l;

	*tag = temp_tag;
	temp_tag->allocated = false;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_tag_byte_array(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));
	int32_t size = 0;

	if (id_prefixed && next(state) != TAG_BYTE_ARRAY)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_BYTE_ARRAY;

	READ_NAME

	bytearray_init(&temp_tag->tag_byte_array);
	if (state->needle + 4 >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	size |= (int32_t)next(state) << 24;
	size |= (int32_t)next(state) << 16;
	size |= (int32_t)next(state) << 8;
	size |= next(state);

	if (state->needle + size >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	bytearray_append_data(&temp_tag->tag_byte_array, state->data + state->needle + 1, size);

	state->needle += size;

	temp_tag->allocated = true;
	*tag = temp_tag;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_tag_string(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));
	uint32_t string_size = 0;

	if (id_prefixed && next(state) != TAG_STRING)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_STRING;

	READ_NAME

	if (state->needle + 2 >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	string_size |= (uint16_t)next(state) << 8;
	string_size |= next(state);

	if (state->needle + string_size >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->tag_string = blue_malloc(string_size + 1);

	memmove(temp_tag->tag_string, state->data + state->needle + 1, string_size);

	temp_tag->tag_string[string_size] = 0;

	state->needle += string_size;

	temp_tag->allocated = true;
	*tag = temp_tag;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_tag_list(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));
	size_t i;

	if (id_prefixed && next(state) != TAG_LIST)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_LIST;

	READ_NAME

	if (state->needle + 5 >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->tag_list.type = next(state);

	temp_tag->tag_list.size = 0;
	temp_tag->tag_list.size |= (int32_t)next(state) << 24;
	temp_tag->tag_list.size |= (int32_t)next(state) << 16;
	temp_tag->tag_list.size |= (int32_t)next(state) << 8;
	temp_tag->tag_list.size |= next(state);

	temp_tag->tag_list.tags = blue_malloc(sizeof(nbt_tag *) * temp_tag->tag_list.size);

#define READCASE(upper_type, lower_type)                                                     \
	case TAG_##upper_type:                                                                   \
		for (i = 0; i < temp_tag->tag_list.size; i++)                                        \
		{                                                                                    \
			if (read_nbt_tag_##lower_type(&temp_tag->tag_list.tags[i], state, false, false)) \
			{                                                                                \
				blue_free(temp_tag->tag_list.tags);                                          \
				return READ_RESULT_MALFORMED;                                                \
			}                                                                                \
		}

	switch (temp_tag->tag_list.type)
	{
		READCASE(BYTE, byte)
		break;
		READCASE(SHORT, short)
		break;
		READCASE(INT, int)
		break;
		READCASE(LONG, long)
		break;
		READCASE(FLOAT, float)
		break;
		READCASE(DOUBLE, double)
		break;
		READCASE(BYTE_ARRAY, byte_array)
		temp_tag->allocated = true;
		break;
		READCASE(STRING, string)
		temp_tag->allocated = true;
		break;
		READCASE(LIST, list)
		temp_tag->allocated = true;
		break;
		case TAG_COMPOUND:
			for (i = 0; i < temp_tag->tag_list.size; i++)
			{
				if (read_nbt_tag_compound(&temp_tag->tag_list.tags[i], state, false, false, true))
				{
					blue_free(temp_tag->tag_list.tags);
					return READ_RESULT_MALFORMED;
				}
			}
			temp_tag->allocated = true;
			break;
			READCASE(INT_ARRAY, int_array)
			temp_tag->allocated = true;
			break;
			READCASE(LONG_ARRAY, long_array)
			temp_tag->allocated = true;
			break;
		default:
			blue_free(temp_tag->tag_list.tags);
			return READ_RESULT_MALFORMED;
	}

	*tag = temp_tag;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_tag_compound(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed, bool ending_nul_byte)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));
	nbt_tag *read_tag_buffer;

	bool done = false;
	if (id_prefixed && next(state) != TAG_COMPOUND)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_COMPOUND;
	temp_tag->tag_compound.size = 0;
	temp_tag->tag_compound.tags = blue_malloc(0);

	READ_NAME;

	while (!done)
	{
		if (state->needle + 1 >= state->data_size)
		{
			if (ending_nul_byte)
			{
				blue_free(temp_tag);
				return READ_RESULT_MALFORMED;
			}
			else
			{
				done = true;
				continue;
			}
		}

		if (peek(state) == 0)
		{
			next(state);
			done = true;
		}
		else
		{
			if (read_tag(&read_tag_buffer, state, true))
			{
				nbt_free(temp_tag);
				return READ_RESULT_MALFORMED;
			}
			temp_tag->tag_compound.size++;
			temp_tag->tag_compound.tags = blue_realloc(temp_tag->tag_compound.tags, temp_tag->tag_compound.size * sizeof(nbt_tag *));
			temp_tag->tag_compound.tags[temp_tag->tag_compound.size - 1] = read_tag_buffer;
		}
	}

	temp_tag->allocated = true;
	*tag = temp_tag;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_tag_int_array(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));
	int32_t size = 0;

	if (id_prefixed && next(state) != TAG_INT_ARRAY)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_INT_ARRAY;

	READ_NAME

	if (state->needle + 4 >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	size |= (int32_t)next(state) << 24;
	size |= (int32_t)next(state) << 16;
	size |= (int32_t)next(state) << 8;
	size |= next(state);

	if (state->needle + size >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->tag_int_array.size = size;
	temp_tag->tag_int_array.ints = blue_malloc(sizeof(int32_t) * size);

	memmove(temp_tag->tag_int_array.ints, state->data + state->needle + 1, size);

	state->needle += size;

	temp_tag->allocated = true;
	*tag = temp_tag;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_tag_long_array(nbt_tag **tag, read_state *state, bool has_name, bool id_prefixed)
{
	nbt_tag *temp_tag = blue_malloc(sizeof(nbt_tag));
	int32_t size = 0;

	if (id_prefixed && next(state) != TAG_LONG_ARRAY)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->type = TAG_LONG_ARRAY;

	READ_NAME

	if (state->needle + 4 >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	size |= (int32_t)next(state) << 24;
	size |= (int32_t)next(state) << 16;
	size |= (int32_t)next(state) << 8;
	size |= next(state);

	if (state->needle + size >= state->data_size)
	{
		blue_free(temp_tag);
		return READ_RESULT_MALFORMED;
	}

	temp_tag->tag_long_array.size = size;
	temp_tag->tag_long_array.longs = blue_malloc(sizeof(int64_t) * size);

	memmove(temp_tag->tag_long_array.longs, state->data + state->needle + 1, size);

	state->needle += size;

	temp_tag->allocated = true;
	*tag = temp_tag;
	return READ_RESULT_SUCCESS;
}

static read_result read_nbt_string(read_state *state, char **str)
{
	uint16_t size = 0;
	if (state->needle + 2 >= state->data_size)
	{
		return READ_RESULT_MALFORMED;
	}

	size |= (((uint32_t)next(state)) << 8);
	size |= next(state);

	if (state->needle + size >= state->data_size)
	{
		return READ_RESULT_MALFORMED;
	}

	str = blue_malloc(size * sizeof(uint8_t) + 1);

	memmove(str, state->data + state->needle + 1, size);
	state->needle += size;

	str[size] = 0;

	return READ_RESULT_SUCCESS;
}

static uint8_t next(read_state *state)
{
	state->needle += 1;
	return state->data[state->needle];
}

static uint8_t peek(read_state *state)
{
	return state->data[state->needle + 1];
}

char *str_repeat(char *str, size_t count)
{
	if (count == 0)
		return NULL;
	char *ret = blue_malloc(strlen(str) * count + count);
	if (ret == NULL)
		return NULL;
	strcpy(ret, str);
	while (--count > 0)
	{
		strcat(ret, " ");
		strcat(ret, str);
	}
	return ret;
}

void nbt_dump(nbt_tag *tag, uint32_t indent)
{
	size_t i;
	char *name;
	char *tabs = str_repeat("\t", indent);
	if (tag->name != NULL)
	{
		name = strdup(tag->name);
	}
	else
	{
		name = strdup("<NO NAME>");
	}

	printf("%s", tabs);

	switch (tag->type)
	{
		case TAG_BYTE:
			printf("TAG BYTE[name: \"%s\"](%u)\n", name, tag->tag_byte);
			break;
		case TAG_SHORT:
			printf("TAG SHORT[name: \"%s\"](%u)\n", name, tag->tag_short);
			break;
		case TAG_INT:
			printf("TAG INT[name: \"%s\"](%i)\n", name, tag->tag_int);
			break;
		case TAG_LONG:
			printf("TAG LONG[name: \"%s\"](%ld)\n", name, tag->tag_long);
			break;
		case TAG_FLOAT:
			printf("TAG FLOAT[name: \"%s\"](%f)\n", name, tag->tag_float);
			break;
		case TAG_DOUBLE:
			printf("TAG DOUBLE[name: \"%s\"](%f)\n", name, tag->tag_double);
			break;
		case TAG_BYTE_ARRAY:
			printf("TAG BYTE ARRAY(SIZE: %lu)[name: \"%s\"] {\n%s\t", tag->tag_byte_array.size, name, tabs);
			for (i = 0; i < tag->tag_byte_array.size; i++)
			{
				if (i == tag->tag_byte_array.size - 1)
				{
					printf("BYTE(%u)\n", tag->tag_byte_array.data[i]);
				}
				else
				{
					printf("BYTE(%u), ", tag->tag_byte_array.data[i]);
				}
			}
			printf("%s}\n", tabs);
			break;
		case TAG_STRING:
			printf("TAG STRING(SIZE: %lu)[name: \"%s\"] {\"%s\"}\n", strlen(tag->tag_string), name, tag->tag_string);
			break;
		case TAG_LIST:
			printf("TAG LIST(SIZE: %lu)[name: \"%s\"](TYPE: %s) {\n", tag->tag_list.size, name, tag_type_to_string(tag->tag_list.type));
			for (i = 0; i < (size_t)tag->tag_list.size; i++)
			{
				nbt_dump(tag->tag_list.tags[i], indent + 1);
			}
			printf("%s}\n", tabs);
			break;
		case TAG_COMPOUND:
			printf("TAG COMPOUND(SIZE: %lu)[name: \"%s\"] {\n", tag->tag_compound.size, name);
			for (i = 0; i < (size_t)tag->tag_compound.size; i++)
			{
				nbt_dump(tag->tag_compound.tags[i], indent + 1);
			}
			printf("%s}\n", tabs);
			break;
		case TAG_INT_ARRAY:
			printf("TAG INT ARRAY(SIZE: %lu)[name: \"%s\"] {\n", tag->tag_int_array.size, name);
			for (i = 0; i < (size_t)tag->tag_int_array.size; i++)
			{
				if (i == (size_t)tag->tag_int_array.size - 1)
				{
					printf("INT(%u)\n", tag->tag_int_array.ints[i]);
				}
				else
				{
					printf("INT(%u), ", tag->tag_int_array.ints[i]);
				}
			}
			printf("%s}\n", tabs);
			break;
		case TAG_LONG_ARRAY:
			printf("TAG LONG ARRAY(SIZE: %lu)[name: \"%s\"] {\n", tag->tag_long_array.size, name);
			for (i = 0; i < (size_t)tag->tag_long_array.size; i++)
			{
				if (i == (size_t)tag->tag_long_array.size - 1)
				{
					printf("LONG(%ld)\n", tag->tag_long_array.longs[i]);
				}
				else
				{
					printf("LONG(%ld), ", tag->tag_long_array.longs[i]);
				}
			}
			printf("%s}\n", tabs);
			break;
	}

	blue_free(name);
	blue_free(tabs);
}

const char *tag_type_to_string(nbt_tag_type type)
{
	switch (type)
	{
		case TAG_BYTE:
			return "BYTE";
		case TAG_SHORT:
			return "SHORT";
		case TAG_INT:
			return "INT";
		case TAG_LONG:
			return "LONG";
		case TAG_FLOAT:
			return "FLOAT";
		case TAG_DOUBLE:
			return "DOUBLE";
		case TAG_BYTE_ARRAY:
			return "BYTE_ARRAY";
		case TAG_STRING:
			return "STRING";
		case TAG_LIST:
			return "LIST";
		case TAG_COMPOUND:
			return "COMPOUND";
		case TAG_INT_ARRAY:
			return "INT_ARRAY";
		case TAG_LONG_ARRAY:
			return "LONG_ARRAY";
	}
	return "UNKNOWN";
}
