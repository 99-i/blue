#include "net/client.h"
#include "game/client_event.h"
#include "mem.h"
#include "net/server.h"
#include "packet/liaison.h"
#include "packet/protocol.h"
#include "types.h"
#include "util/bytearray.h"
#include "uv.h"
#include "uv/unix.h"
#include <cstddef>
#include <pthread.h>
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
	if (nread == UV_EOF)
	{
		printf("Got EOF. Client closing.\n");
		server_client_disconnect(c->s, c);
	}
	else if (nread < 0)
	{
		/* error */
		server_client_disconnect(c->s, c);
	}
	else
	{
		client_handle_read(c, (const uint8_t *)buf->base, nread);
	}

	if (buf->base != NULL && buf->len != 0)
	{
		blue_free(buf->base);
	}
}

static void client_handle_read(client *c, const uint8_t *data, size_t size)
{
	liaison_append_data(c->liaison, data, size);

	client_check_events(c);
}

static void client_check_events(client *c)
{
	client_event event;
	read_result result;

	while (!(result = liaison_pop_client_event(c->liaison, &event)))
	{
		printf("Got game event %d\n", event.type);
		if (event.type == CLIENT_EVENT_HANDSHAKE)
		{
			switch (event.handshake.next_state)
			{
				case 1:
					liaison_set_client_state(c->liaison, STATE_STATUS);
					break;
				case 2:
					liaison_set_client_state(c->liaison, STATE_LOGIN);
					break;
			}
			switch (event.handshake.protocol_version)
			{
				case 47:
					liaison_set_version(c->liaison, PROTOCOL_47);
					break;
				default:
					/* unsupported */
					break;
			}
		}
	}

	if (result == READ_RESULT_MALFORMED)
	{
		printf("MALFORMED RESULT!\n");
	}
}

void client_start(client *c)
{
	c->liaison = liaison_create(c);
	c->tcp_handle.data = c;
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
		printf("ERROR: WRITE FAILED. %s", uv_strerror(status));
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

void client_send_raw_bytearray(client *c, const bytearray *arr)
{
	client_send_raw_bytes(c, arr->data, arr->size);
}
