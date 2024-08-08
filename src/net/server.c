#include "net/server.h"
#include "memory.h"
#include "net/client.h"
#include "uv.h"

static void connection_cb(uv_stream_t *stream, int status)
{
	server *s = stream->data;

	if (status < 0)
	{
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
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

	uv_tcp_init(&s->server_loop, &c->tcp_handle);

	if (uv_accept((uv_stream_t *)&s->listen_handle,
				  (uv_stream_t *)&c->tcp_handle) == 0)
	{
		client_start(c);
	}
	else
	{
		printf("Closing Client.\n");
		uv_close((uv_handle_t *)&c->tcp_handle, client_stream_close_cb);
		return;
	}

	if (s->clients.size == s->clients.capacity)
	{
		s->clients.clients =
			realloc(s->clients.clients,
					(s->clients.capacity == 0 ? 1 : s->clients.capacity * 2) *
						sizeof(client *));
		s->clients.clients[s->clients.size] = c;
	}
}

bool server_init(server *s, server_settings *settings)
{
	int result;
	struct sockaddr_in address;
	result = uv_loop_init(&s->server_loop);
	result = uv_tcp_init(&s->server_loop, &s->listen_handle);

	uv_ip4_addr("0.0.0.0", 25565, &address);

	uv_tcp_bind(&s->listen_handle, (const struct sockaddr *)&address, 0);

	result = uv_listen((uv_stream_t *)&s->listen_handle, 512, connection_cb);

	if (result)
		return false;

	s->clients.capacity = 32;
	s->clients.clients = blue_malloc(s->clients.capacity * sizeof(client *));
	s->clients.size = 0;

	s->listen_handle.data = s;

	memcpy(&s->settings, settings, sizeof(server_settings));

	return true;
}

void server_client_disconnect(server *s, client *c)
{
	size_t i;
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
}

void server_run_thread(void *data)
{
	uv_run(&((server *)data)->server_loop, UV_RUN_DEFAULT);
}

void server_run(server *s)
{
	uv_thread_create(&s->run_thread, server_run_thread, s);
}

void server_join(server *s)
{
	uv_thread_join(&s->run_thread);
}
