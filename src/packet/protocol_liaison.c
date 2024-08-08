#include "packet/protocol_liaison.h"
#include "memory.h"
#include "packet/protocol_47.h"
#include <assert.h>

protocol_liaison *construct_protocol_liaison(protocol_version version)
{
	protocol_liaison *liaison = blue_malloc(sizeof(protocol_liaison));

	liaison->version = version;

	switch (version)
	{
		case UNDECIDED:
			blue_free(liaison);
			return NULL;
		case PROTOCOL_47:
			liaison->state = protocol_47_create_state();
			liaison->append_data = protocol_47_append_data;
			liaison->pop_game_event = protocol_47_pop_game_event;
			return liaison;
		case PROTOCOL_5:
			blue_free(liaison);
			assert(false);
			return NULL;
	}
}

void liaison_append_data(protocol_liaison *liaison, const uint8_t *data, size_t size)
{
	return liaison->append_data(liaison->state, data, size);
}

read_result liaison_pop_game_event(protocol_liaison *liaison, game_event *event)
{
	return liaison->pop_game_event(liaison->state, event);
}
