#pragma once
#include "cJSON.h"
#include "util/string.h"
#include <stdint.h>

typedef struct
{
	float x;
	float y;
	float z;
} fpos;

typedef struct
{
	int32_t x;
	int32_t y;
	int32_t z;
} ipos;

typedef int64_t uuid[2]; /* msb, then lsb. */

typedef struct
{
	string raw_json;
	cJSON *json;
} chat_obj;

typedef enum
{
	READ_RESULT_SUCCESS = 0,
	READ_RESULT_NOT_ENOUGH_DATA,
	READ_RESULT_MALFORMED
} read_result;
