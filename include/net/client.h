#pragma once
#include "game/entity.h"
#include "packet/liaison.h"
#include "packet/protocol.h"
#include "util/bytearray.h"
#include "uv.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct client_s
{
	uv_tcp_t tcp_handle;
	struct s_server *s;
	liaison *liaison;

	bool disconnect;

	bool ingame;  /* if the client is in-game and has a player. */
	entity_id id; /* NULL if not connected. */

} client;

void client_start(client *c);

void client_send_raw_bytes(client *c, const uint8_t *bytes, size_t size);

void client_send_raw_bytearray(client *c, const bytearray *arr);

bool client_check_disconnect(client *c);

void client_free(client *c);
