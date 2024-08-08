#include "packet/packet.h"
#include "packet/packet_read_fn.h"
#include "packet/read_fn.h"

static read_result read_packet(client_state state, bytearray *data, size_t offset, serverbound_packet *packet);

read_result packet_pop(client_state state, bytearray *data, serverbound_packet *packet)
{
	uint32_t size;
	int32_t packet_length;
	read_result result;
	bytearray temporary_packet_buffer;

	result = read_varint(data, 0, &packet_length, &size);
	if (!result)
		return result;

	if (data->size - size < (size_t)packet_length)
	{
		return READ_RESULT_NOT_ENOUGH_DATA;
	}

	printf("Read packet with length %d\n", packet_length);

	/* construct temporary buffer with capacity of packet length. */

	temporary_packet_buffer.capacity = packet_length;
	temporary_packet_buffer.size = packet_length;
	temporary_packet_buffer.data = data->data;

	return read_packet(state, &temporary_packet_buffer, size, packet);
}

static read_result read_packet(client_state state, bytearray *data, size_t offset, serverbound_packet *packet)
{
	int32_t id;
	uint32_t size;
	read_result result;

	static const read_fn read_handshaking[1] = {read_handshaking_0};
	static const read_fn read_status[2] = {read_status_0, read_status_1};
	static const read_fn read_login[2] = {read_login_0, read_login_1};
	static const read_fn read_play[26] = {
		read_play_0,
		read_play_1,
		read_play_2,
		read_play_3,
		read_play_4,
		read_play_5,
		read_play_6,
		read_play_7,
		read_play_8,
		read_play_9,
		read_play_10,
		read_play_11,
		read_play_12,
		read_play_13,
		read_play_14,
		read_play_15,
		read_play_16,
		read_play_17,
		read_play_18,
		read_play_19,
		read_play_20,
		read_play_21,
		read_play_22,
		read_play_23,
		read_play_24,
		read_play_25,
	};

	result = read_varint(data, offset, &id, &size);
	if (!result)
		return result;

	offset += size;

	switch (state)
	{
		case STATE_HANDSHAKING:
			return read_handshaking[id](data, offset, packet, &size);
		case STATE_STATUS:
			return read_status[id](data, offset, packet, &size);
		case STATE_LOGIN:
			return read_login[id](data, offset, packet, &size);
		case STATE_PLAY:
			return read_play[id](data, offset, packet, &size);
	}
	return READ_RESULT_MALFORMED;
}
