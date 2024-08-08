#pragma once
#include "nbt.h"
#include <stdint.h>

typedef struct
{
	int16_t id;
	int8_t count;
	int16_t damage;
	nbt_tag item;
} slot_data;
