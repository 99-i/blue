#include "nbt.h"
#include "util/bytearray.h"
#include <stdio.h>
#include <stdlib.h>

static void usage(void)
{
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		usage();
		return 0;
	}
	char *fn = argv[1];
	FILE *f = fopen(fn, "rb");
	long size;
	bytearray buffer;
	read_result result;
	nbt_tag *tag;

	if (!f)
	{
		printf("Could not open file bigtest.nbt");
		return 1;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);

	buffer.data = malloc(size);

	/* copy the file into the buffer */
	fread(buffer.data, size, 1, f);

	buffer.size = size;
	buffer.capacity = size;

	result = nbt_read(&tag, &buffer, 0);

	if (result)
	{
		printf("nbt_read return non-successful error code %d", result);
		return result;
	}

	nbt_dump(tag, 0);

	return 0;
}
