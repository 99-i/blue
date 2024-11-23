#include "net/server.h"
#include "game/game.h"
#include "log.h"
#include "mem.h"
#include "net/client.h"
#include "packet/protocol.h"
#include "packet/slp.h"
#include "uv.h"
#include <stdio.h>
#include <string.h>

static void connection_cb(uv_stream_t *stream, int status)
{
	server *s = stream->data;

	if (status < 0)
	{
		log_error("New connection error. %s", uv_strerror(status));
		return;
	}

	server_accept_client(s);
}

static void client_stream_close_cb(uv_handle_t *handle)
{
	blue_free(handle->data);
}

void server_accept_client(server *s)
{
	client *c = blue_malloc(sizeof(client));
	c->s = s;
	size_t new_capacity;

	uv_tcp_init(&s->server_loop, &c->tcp_handle);
	c->tcp_handle.data = c;

	if (uv_accept((uv_stream_t *)&s->listen_handle,
				  (uv_stream_t *)&c->tcp_handle) == 0)
	{
		client_start(c);
	}
	else
	{
		uv_close((uv_handle_t *)&c->tcp_handle, client_stream_close_cb);
		return;
	}

	uv_mutex_lock(&s->clients_mutex);
	if (s->clients.size == s->clients.capacity)
	{
		new_capacity = (s->clients.capacity == 0 ? 1 : s->clients.capacity * 2);
		s->clients.clients = blue_realloc(s->clients.clients, new_capacity * sizeof(client *));
		s->clients.capacity = new_capacity;
	}
	s->clients.clients[s->clients.size] = c;
	s->clients.size++;
	uv_mutex_unlock(&s->clients_mutex);
}

bool server_init(server *s, server_settings *settings)
{
	int result;
	struct sockaddr_in address;
	result = uv_loop_init(&s->server_loop);
	result = uv_tcp_init(&s->server_loop, &s->listen_handle);

	uv_ip4_addr("0.0.0.0", 25565, &address);

	uv_tcp_bind(&s->listen_handle, (const struct sockaddr *)&address, 0);

	uv_tcp_nodelay(&s->listen_handle, 1);

	s->listen_handle.data = s;

	result = uv_listen((uv_stream_t *)&s->listen_handle, 512, connection_cb);

	if (result)
		return false;

	s->clients.capacity = 32;
	s->clients.clients = blue_malloc(s->clients.capacity * sizeof(client *));
	s->clients.size = 0;

	memcpy(&s->settings, settings, sizeof(server_settings));

	uv_mutex_init(&s->clients_mutex);

	game_init(&s->g);

	return true;
}

void server_remove_client(server *s, client *c)
{
	size_t i;
	uv_mutex_lock(&s->clients_mutex);
	for (i = 0; i < s->clients.size; i++)
	{
		if (s->clients.clients[i] == c)
		{
			if (i != s->clients.size - 1)
			{
				memmove(&s->clients.clients[i], &s->clients.clients[i + 1],
						s->clients.size - i - 1);
			}
			s->clients.size--;
		}
	}
	uv_mutex_unlock(&s->clients_mutex);
}

bool server_supports_protocol_version(server *s, int32_t version)
{
	if (!is_protocol_version_supported(version))
		return false;

	return s->settings.max_version >= version && version >= s->settings.min_version;
}

slp_object *server_get_slp(server *s, protocol_version version)
{
	slp_object *slp = blue_malloc(sizeof(*slp));

	if (server_supports_protocol_version(s, version))
	{
		slp->description = "Welcome to our server.";
		slp->shown_version = version;
	}
	else
	{
		if (s->settings.max_version < version)
		{
			slp->description = "Outdated server.";
		}
		else
		{
			slp->description = "Outdated client.";
		}
		slp->shown_version = s->settings.min_version;
	}

	slp->version_name = blue_malloc(50);
	memset(slp->version_name, 0, 50);

	if (s->settings.min_version == s->settings.max_version)
	{
		snprintf(slp->version_name, 50, "blue %s", protocol_version_to_cstr(s->settings.min_version));
	}
	else
	{
		snprintf(slp->version_name, 50, "blue %s - %s", protocol_version_to_cstr(s->settings.min_version), protocol_version_to_cstr(s->settings.max_version));
	}

	/* blue does not support a max number of players. */
	slp->players.max = 5000;

	slp->players.online = s->clients.size;

	return slp;
}

void server_run_thread(void *data)
{
	log_info("Blue starting up.");
	uv_run(&((server *)data)->server_loop, UV_RUN_DEFAULT);
}

void server_run(server *s)
{
	uv_thread_create(&s->run_thread, server_run_thread, s);
	game_run(&s->g);
}

void server_join(server *s)
{
	uv_thread_join(&s->run_thread);
}
