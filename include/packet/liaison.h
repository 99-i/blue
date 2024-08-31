#pragma once
#include "game/client_event.h"
#include "game/game_event.h"
#include "packet/protocol.h"
#include "packet/slp.h"
#include "types.h"
#include "util/bytearray.h"
#include "uv.h"

typedef struct client_s client;

typedef struct s_liaison
{
	protocol_version version;
	bytearray read_buffer;
	client_state c_state;
	client *c;
	/* pointer to internal state used by the liaison.
	 * for example, 1.8.9 uses compression while 1.7.10 doesn't. */
	void *liaison_state;
} liaison;

/* create an undecided liaison which handles reading the handshake. */
/* must be freed with liaison_free. */
liaison *liaison_create(client *c);

void liaison_append_data(liaison *l, const uint8_t *data, size_t size);

read_result liaison_pop_client_event(liaison *l, client_event *event);

void liaison_send_game_event(liaison *l, game_event *event);

void liaison_free(liaison *l);
