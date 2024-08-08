#include "nbt.h"
#include "packet/packet.h"
#include "util/bytearray.h"
#include <stdio.h>

int main(void)
{
	FILE *f = fopen("bigtest.nbt", "rb");
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
