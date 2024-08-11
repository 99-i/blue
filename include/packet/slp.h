#pragma once

#include "cJSON.h"
#include "packet/protocol.h"
#include "types.h"
#include "util/string.h"

typedef struct
{
	protocol_version version;
	struct
	{
		uint32_t max;
		uint32_t online;
		/* No sample. */
	} players;
	string description;
	/* favicon */
	/* enforces secure chat */
} slp_object;

void slp_free(slp_object *obj);

/* string.data must be freed with blue_free. */
string slp_to_json(slp_object *slp);
