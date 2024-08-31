#pragma once

#include "cJSON.h"
#include "packet/protocol.h"
#include "types.h"

typedef struct
{
	char *version_name;
	protocol_version shown_version;
	struct
	{
		uint32_t max;
		uint32_t online;
		/* No sample. */
	} players;
	const char *description;
	/* favicon */
	/* enforces secure chat */
} slp_object;

void slp_free(slp_object *obj);

/* return value must be freed with blue_free. */
char *slp_to_json(slp_object *slp);
