#pragma once
#include <stdint.h>
#define EMPTY_STRING \
	(string)         \
	{                \
		0, 0         \
	}

typedef struct
{
	wchar_t *data;
	size_t size;
} string;

/* returns a null-terminated version of <s> that must be freed using blue_free. */
char *string_get_cstr(string *s);
