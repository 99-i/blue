#include "packet/protocol.h"
#include "mem.h"
#include "net/server.h"
#include "packet/protocol_47.h"
#include "packet/protocol_undecided.h"
#include "packet/read_fn.h"
#include "types.h"
#include <assert.h>

protocol_undecided_state *protocol_undecided_create_state(void)
{
	protocol_undecided_state *state = blue_malloc(sizeof(*state));
	state->version = 0;
	return state;
}

static read_result protocol_undecided_pop_packet(bytearray *read_buffer, client_state c_state, protocol_undecided_state *state, protocol_undecided_serverbound_packet *packet);

read_result protocol_undecided_pop_client_event(liaison *l, client_event *event)
{
	read_result result;
	protocol_undecided_serverbound_packet packet;

	/* Attempt to pop packet. */
	result = protocol_undecided_pop_packet(&l->read_buffer, l->c_state, l->liaison_state, &packet);

	if (result)
		return result;

	switch (packet.ptype)
	{
		case PROTOCOL_UNDECIDED_HANDSHAKE:
			switch (packet.handshake.next_state)
			{
				case 1:
					/* status */
					l->c_state = STATE_STATUS;
					/* don't set the protocol version of the liaison, so it still falls through to PROTOCOL_UNDECIDED and we can handle it here. There is no way to change the state after it's set to status, so this is fine. */
					/* we can, however, cache it in our liaison's state. */
					/* todo: confusing configuration. */
					((protocol_undecided_state *)l->liaison_state)->version = packet.handshake.protocol_version;
					break;
				case 2:
					/* login */
					l->c_state = STATE_LOGIN;
					if (!server_supports_protocol_version(l->c->s, packet.handshake.protocol_version))
					{
						/* todo: send disconnect packet. */
					}

					l->version = packet.handshake.protocol_version;
					blue_free(l->liaison_state);

					switch (packet.handshake.protocol_version)
					{
						case PROTOCOL_47:
							l->liaison_state = protocol_47_create_state();
							break;
						default:
							assert(false && "unreachable.");
							return READ_RESULT_MALFORMED;
							break;
					}
					break;
			}
			break;
		case PROTOCOL_UNDECIDED_REQUEST:
			{
				/* todo: send slp. */
				bytearray slp_buffer;
			}
			break;
		case PROTOCOL_UNDECIDED_PING:
			{
			}
			break;
	}

	return READ_RESULT_NO_RESULT;
}

static read_result protocol_undecided_pop_packet(bytearray *read_buffer, client_state c_state, protocol_undecided_state *state, protocol_undecided_serverbound_packet *packet)
{
	(void)state;
	uint32_t size;
	uint32_t size_buffer;
	int32_t packet_length;
	read_result result;
	bytearray temporary_packet_buffer;

	result = read_varint(read_buffer, 0, &packet_length, &size);
	if (result)
		return result;

	if (read_buffer->size - size < (size_t)packet_length)
	{
		return READ_RESULT_NOT_ENOUGH_DATA;
	}
	/* construct temporary buffer with size of packet length. */

	temporary_packet_buffer.capacity = packet_length;
	temporary_packet_buffer.size = packet_length;
	temporary_packet_buffer.data = read_buffer->data + size;

	result = protocol_undecided_read_packet(&temporary_packet_buffer, c_state, 0, packet, &size_buffer);
	if (result)
		return result;

	if ((int32_t)size_buffer != packet_length)
	{
		return READ_RESULT_MALFORMED;
	}

	bytearray_slice_from_beginning(read_buffer, size + size_buffer);

	return READ_RESULT_SUCCESS;
}

read_result protocol_undecided_read_packet(bytearray *data, client_state c_state, size_t offset, protocol_undecided_serverbound_packet *packet, uint32_t *bytes_read)
{
	int32_t id;
	uint32_t size;
	read_result result;
	static const protocol_undecided_read_packet_fn read_status[] = {protocol_undecided_read_status_0, protocol_undecided_read_status_1};

	result = read_varint(data, offset, &id, &size);

	if (result)
		return result;

	offset += size;

	switch (c_state)
	{
		case STATE_HANDSHAKING:
			if (id != 0)
				return READ_RESULT_MALFORMED;

			result = protocol_undecided_read_handshaking_0(data, offset, packet, &size);
			break;
		case STATE_STATUS:
			if ((unsigned long)id >= sizeof(read_status) / sizeof(read_status[0]))
			{
				return READ_RESULT_MALFORMED;
			}
			result = read_status[id](data, offset, packet, &size);
			break;
		default:
			assert(false && "unreachable?");
			return READ_RESULT_MALFORMED;
			break;
	}

	if (result)
		return result;

	offset += size;

	if (bytes_read)
	{
		*bytes_read = offset;
	}

	return READ_RESULT_SUCCESS;
}

#define DO_READ(read_fn, variable_name)                                        \
	result = read_fn(data, offset + bytes_read_buffer, &variable_name, &size); \
	if (result)                                                                \
		return result;                                                         \
	bytes_read_buffer += size;

read_result protocol_undecided_read_handshaking_0(bytearray *data, size_t offset, protocol_undecided_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int32_t protocol_version;
	string server_address;
	uint16_t server_port;
	int32_t next_state;

	DO_READ(read_varint, protocol_version);
	DO_READ(read_string, server_address);
	DO_READ(read_u16, server_port);
	DO_READ(read_varint, next_state);

	packet->ptype = PROTOCOL_UNDECIDED_HANDSHAKE;
	packet->handshake.protocol_version = protocol_version;
	packet->handshake.server_address = server_address;
	packet->handshake.server_port = server_port;
	packet->handshake.next_state = next_state;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

read_result protocol_undecided_read_status_0(bytearray *data, size_t offset, protocol_undecided_serverbound_packet *packet, uint32_t *bytes_read)
{
	if (bytes_read)
	{
		*bytes_read = 0;
	}
	packet->ptype = PROTOCOL_UNDECIDED_REQUEST;
	return READ_RESULT_SUCCESS;
}

read_result protocol_undecided_read_status_1(bytearray *data, size_t offset, protocol_undecided_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int64_t payload;

	DO_READ(read_i64, payload);

	packet->ptype = PROTOCOL_UNDECIDED_PING;
	packet->ping.payload = payload;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
