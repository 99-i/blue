#include "net/client.h"
#include "game/client_event.h"
#include "game/entity.h"
#include "log.h"
#include "mem.h"
#include "net/server.h"
#include "packet/liaison.h"
#include "packet/protocol.h"
#include "types.h"
#include "util/bytearray.h"
#include "uv.h"
#include <pthread.h>
#include <stddef.h>
#include <string.h>

static void client_handle_read(client *c, const uint8_t *data, size_t size);
static void client_check_events(client *c);

static void alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	(void)handle;
	buf->base = blue_malloc(suggested_size);
	buf->len = suggested_size;
}

static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	client *c = stream->data;

	if (client_check_disconnect(c))
	{
		goto end;
	}
	else if (nread == UV_EOF)
	{
		/* client sent eof */
		c->disconnect = true;
	}
	else if (nread < 0)
	{
		/* read error */
		c->disconnect = true;
	}
	else
	{
		client_handle_read(c, (const uint8_t *)buf->base, nread);
	}

end:

	if (buf->base != NULL && buf->len != 0)
	{
		blue_free(buf->base);
	}
}

static void client_handle_read(client *c, const uint8_t *data, size_t size)
{
	bytearray arr;
	arr.data = (uint8_t *)data;
	arr.size = size;
	arr.capacity = size;

	liaison_append_data(c->liaison, data, size);

	client_check_events(c);
}

static void client_check_events(client *c)
{
	client_event event;
	read_result result;

	while (true)
	{
		if (c->disconnect)
			break;
		result = liaison_pop_client_event(c->liaison, &event);

		if (result == READ_RESULT_NO_RESULT)
			continue;

		if (result)
		{
			break;
		}

		switch (event.type)
		{
			case CLIENT_EVENT_JOIN:
				{
				}
				break;
			case CLIENT_EVENT_CHAT_MESSAGE:
				{
				}
				break;
		}
	}

	if (result == READ_RESULT_MALFORMED)
	{
	}
}

void client_start(client *c)
{
	c->liaison = liaison_create(c);
	c->tcp_handle.data = c;
	c->disconnect = false;

	c->ingame = false;
	c->id = ENTITY_NULL;

	uv_read_start((uv_stream_t *)&c->tcp_handle, alloc_cb, read_cb);
}

typedef struct
{
	uv_write_t req;
	client *c;
	uv_buf_t buf;
} client_write_request;

static void on_client_write(uv_write_t *req, int status)
{
	client_write_request *request = (client_write_request *)((char *)req - offsetof(client_write_request, req));

	if (status < 0)
	{
		log_error("Write failed. %s", uv_strerror(status));
	}

	blue_free(request->buf.base);
	blue_free(request);
}

void client_send_raw_bytes(client *c, const uint8_t *bytes, size_t size)
{
	client_write_request *request = blue_malloc(sizeof(*request));
	request->c = c;

	request->buf.base = blue_malloc(size);
	memcpy(request->buf.base, bytes, size);
	request->buf.len = size;

	uv_write(&request->req, (uv_stream_t *)&c->tcp_handle, &request->buf, 1, on_client_write);
}

/* TODO: turn this into a macro */
void client_send_raw_bytearray(client *c, const bytearray *arr)
{
	client_send_raw_bytes(c, arr->data, arr->size);
}

bool client_check_disconnect(client *c)
{
	if (c->disconnect)
	{
		server_remove_client(c->s, c);
		client_free(c);
		return true;
	}
	return false;
}

/* only now can we free the client. */
void on_client_tcp_handle_close(uv_handle_t *handle)
{
	uv_tcp_t *tcp = (uv_tcp_t *)handle;
	client *c = (client *)(((char *)tcp) - offsetof(client, tcp_handle));

	blue_free(c);
}

void client_free(client *c)
{
	liaison_free(c->liaison);
	uv_close((uv_handle_t *)&c->tcp_handle, on_client_tcp_handle_close);
}
