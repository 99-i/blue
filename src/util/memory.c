#include "log.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

void *blue_malloc(size_t size)
{
	void *ptr = malloc(size);
	if (ptr == NULL)
	{
		log_fatal("Could not allocate memory. Shutting down.");
		/* TODO: exit behaviors. */
		exit(1);
	}
	return ptr;
}

void *blue_realloc(void *ptr, size_t new_size)
{
	void *new_ptr = realloc(ptr, new_size);
	if (new_ptr == NULL)
	{
		log_fatal("Could not allocate memory. Shutting down.");
		/* TODO: exit behaviors. */
		exit(1);
	}
	return new_ptr;
}

void blue_free(void *ptr)
{
	free(ptr);
}
