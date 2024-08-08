#include "util/string.h"
#include "memory.h"
#include <string.h>

char *string_get_cstr(string *s)
{
	char *ptr = blue_malloc(s->size + 1);
	memcpy(ptr, s->data, s->size);
	ptr[s->size] = 0;

	return ptr;
}
