#pragma once
#include <stddef.h>
#define EMPTY_STRING \
	(string)         \
	{                \
		0, 0         \
	}

typedef struct
{
	wchar_t *data;
	size_t size; /* in bytes */
} string;

/* returns a null-terminated version of <s> that must be freed using blue_free. */
char *string_get_cstr(string *s);

/* returns a string from a cstr. string->data must be freed using blue_free */
string string_from_cstr(const char *cstr);

/* returns a string from a wcstr. string->data must be freed using blue_free */
string string_from_wcstr(const wchar_t *cstr);
