#pragma once
#include "game/event.h"
#include "net/protocol.h"
#include "types.h"

typedef struct s_protocol_liaison
{
	protocol_version version;

	/* pointer to internal state used by the liaison.
	 * also stores the data buffer sent from pop_game_event.
	 * for example, 1.8.9 uses compression while 1.7.10 doesn't. */
	void *state;

	/* add <data> to the internal data buffer */
	void (*append_data)(void *state, const uint8_t *data, size_t size);

	/* attempt to pop a packet
	 * if malformed, return READ_RESULT_MALFORMED
	 * if not enough data, return READ_RESULT_NOT_ENOUGH_DATA */
	read_result (*pop_game_event)(void *state, game_event *event);
} liaison;

liaison *liaison_create(void); /* create an undecided liaison which handles reading the handshake. */

void liaison_set_version(protocol_version version);
