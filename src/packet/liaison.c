#include "packet/liaison.h"
#include "mem.h"
#include "net/client.h"
#include "packet/protocol.h"
#include "packet/protocol_47.h"
#include "packet/protocol_undecided.h"
#include "packet/read_fn.h"
#include "types.h"
#include "util/bytearray.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

liaison *liaison_create(client *c)
{
	liaison *l = blue_malloc(sizeof(*l));

	l->version = PROTOCOL_UNDECIDED;
	l->liaison_state = protocol_undecided_create_state();
	l->c_state = STATE_HANDSHAKING;
	l->c = c;
	bytearray_init(&l->read_buffer);

	return l;
}

void liaison_append_data(liaison *l, const uint8_t *data, size_t size)
{
	bytearray_append_data(&l->read_buffer, data, size);
}

read_result liaison_pop_client_event(liaison *l, client_event *event)
{
	switch (l->version)
	{
		case PROTOCOL_UNDECIDED:
			return protocol_undecided_pop_client_event(l, event);
			break;
		case PROTOCOL_5:
			break;
		case PROTOCOL_47:
			return protocol_47_pop_client_event(l, event);
			break;
	}
	assert(false && "unreachable");
	return READ_RESULT_MALFORMED;
}

void liaison_set_client_state(liaison *l, client_state new_state)
{
	printf("Switch state to %d\n", new_state);
	l->c_state = new_state;
}

#define DO_READ(read_fn, variable_name)                                      \
	result = read_fn(read_buffer, bytes_read_buffer, &variable_name, &size); \
	if (result)                                                              \
		return result;                                                       \
	bytes_read_buffer += size;

/* only purpose is to read the handshaking packet. */
read_result handshaking_liaison_pop_client_event(void *state, client_state c_state, bytearray *read_buffer, client_event *event)
{
	(void)state;
	(void)c_state;

	int32_t length;
	int32_t id;
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int32_t protocol_version;
	string server_address;
	uint16_t server_port;
	int32_t next_state;

	DO_READ(read_varint, length);

	if (length + bytes_read_buffer > read_buffer->size)
	{
		return READ_RESULT_NOT_ENOUGH_DATA;
	}
	DO_READ(read_varint, id);
	if (id != 0)
	{
		return READ_RESULT_MALFORMED;
	}

	DO_READ(read_varint, protocol_version);
	DO_READ(read_string, server_address);
	DO_READ(read_u16, server_port);
	DO_READ(read_varint, next_state);

	event->type = CLIENT_EVENT_HANDSHAKE;
	event->handshake.protocol_version = protocol_version;
	event->handshake.server_address = server_address;
	event->handshake.server_port = server_port;
	event->handshake.next_state = next_state;

	free(server_address.data);

	bytearray_slice_from_beginning(read_buffer, bytes_read_buffer);

	return READ_RESULT_SUCCESS;
}
