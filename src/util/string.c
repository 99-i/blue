#include "util/string.h"
#include "mem.h"
#include <string.h>
#include <wchar.h>

char *string_get_cstr(string *s)
{
	char *ptr = blue_malloc(s->size + 1);
	memcpy(ptr, s->data, s->size);
	ptr[s->size] = 0;

	return ptr;
}

string string_from_cstr(const char *cstr)
{
	string s;
	size_t len = strlen(cstr);
	s.data = blue_malloc(len);
	memcpy(s.data, cstr, len);
	s.size = len;

	return s;
}

string string_from_wcstr(const wchar_t *wcstr)
{
	string s;
	size_t len = wcslen(wcstr);
	s.data = blue_malloc(len);
	memcpy(s.data, wcstr, len);
	s.size = len;

	return s;
}
