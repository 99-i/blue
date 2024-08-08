#include "net/client.h"
#include "memory.h"
#include "net/server.h"
#include "packet/packet.h"

static void client_handle_read(client *c, const uv_buf_t *buf);
static void client_check_packets(client *c);

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
		client_handle_read(c, buf);
	}

	if (buf->base != NULL && buf->len != 0)
	{
		blue_free(buf->base);
	}
}

static void client_handle_read(client *c, const uv_buf_t *buf)
{
	bytearray_append_uvbuf(&c->read_buffer, buf);
	client_check_packets(c);
}

static void client_check_packets(client *c)
{
	serverbound_packet packet;
	read_result result;

	while (!(result = packet_pop(c->state, &c->read_buffer, &packet)))
	{
		/* Handle packet */
		/* todo */
	}

	if (result == READ_RESULT_MALFORMED)
	{
		/* malformed! */
		printf("Client malformed data\n");
	}
}

void client_start(client *c)
{
	c->tcp_handle.data = c;
	c->state = STATE_HANDSHAKING;
	bytearray_init(&c->read_buffer);
	uv_read_start((uv_stream_t *)&c->tcp_handle, alloc_cb, read_cb);
}
