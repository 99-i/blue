#pragma once
#include <stdint.h>

void *blue_malloc(size_t size);
void *blue_realloc(void *ptr, size_t new_size);
void blue_free(void *ptr);
