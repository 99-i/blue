#pragma once

#include "cJSON.h"
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

typedef struct
{
	char *raw_json;
	cJSON *json;
} chat_obj;

typedef enum
{
	READ_RESULT_SUCCESS = 0,
	READ_RESULT_NOT_ENOUGH_DATA,
	READ_RESULT_MALFORMED,
	READ_RESULT_NO_RESULT /* Read was successful, but the function returning this has nothing to output. */
} read_result;
