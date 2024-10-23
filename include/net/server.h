#pragma once
#include "game/game.h"
#include "net/client.h"
#include "packet/protocol.h"
#include "packet/protocol_undecided.h"
#include "packet/slp.h"
#include "uv/unix.h"
#include <stdbool.h>
#include <uv.h>

typedef struct
{
	/* TODO: excluding versions in between this. */
	protocol_version min_version;
	protocol_version max_version;
	bool online;
} server_settings;

typedef struct s_server
{
	uv_tcp_t listen_handle;
	uv_thread_t run_thread;
	uv_loop_t server_loop;

	uv_mutex_t clients_mutex;
	struct
	{
		size_t size;
		client **clients;
		size_t capacity;
	} clients;

	server_settings settings;

	game g;
} server;

bool server_init(server *s, server_settings *settings);

void server_run(server *s);

void server_join(server *s);

void server_accept_client(server *s);

void server_remove_client(server *s, client *c);

/* if the server settings allow this version
 * to connect to it. also checks with is_protocol_version_supported
 * to see if its even possible (e.g. a liaison exists for that version). */
bool server_supports_protocol_version(server *s, int32_t version);

/* the slp_object* returned by this function must be freed with slp_free. */
slp_object *server_get_slp(server *s, protocol_version version);
