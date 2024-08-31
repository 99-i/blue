#include "cJSON.h"
#include "mem.h"
#include "packet/protocol.h"
#include "packet/slp.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

bool is_protocol_version_supported(int32_t version)
{
	switch (version)
	{
		case 5:
			return true;
		case 47:
			return true;
		default:
			return false;
	}
}

const char *protocol_version_to_cstr(protocol_version version)
{
	switch (version)
	{
		case PROTOCOL_UNDECIDED:
			return "UNDECIDED";
		case PROTOCOL_5:
			return "1.7.10"; /* looks cool */
		case PROTOCOL_47:
			return "1.8.9";
	}
}

void slp_free(slp_object *obj)
{
	blue_free(obj->version_name);
	blue_free(obj);
}

char *slp_to_json(slp_object *slp)
{
	char *s;
	cJSON *total = cJSON_CreateObject();

	cJSON *version = cJSON_CreateObject();
	cJSON *players = cJSON_CreateObject();

	cJSON *description_obj = cJSON_CreateObject();

	cJSON_AddStringToObject(version, "name", slp->version_name);
	cJSON_AddNumberToObject(version, "protocol", (double)slp->shown_version);

	cJSON_AddNumberToObject(players, "max", (double)slp->players.max);
	cJSON_AddNumberToObject(players, "online", (double)slp->players.online);

	cJSON_AddStringToObject(description_obj, "text", (char *)slp->description);

	cJSON_AddItemToObject(total, "version", version);
	cJSON_AddItemToObject(total, "players", players);

	cJSON_AddItemToObject(total, "description", description_obj);

	s = cJSON_PrintUnformatted(total);

	cJSON_Delete(total);

	return s;
}
