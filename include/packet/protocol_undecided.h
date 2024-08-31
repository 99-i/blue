#pragma once

#include "packet/liaison.h"
#include "packet/protocol.h"
#include "types.h"

typedef struct
{
	/* the version of the client as reported in handshake packet.
	 * used to customize SLP response. */
	int32_t version;
} protocol_undecided_state;

protocol_undecided_state *protocol_undecided_create_state(void);

read_result protocol_undecided_pop_client_event(liaison *l, client_event *event);

typedef enum
{
	PROTOCOL_UNDECIDED_HANDSHAKE,
	PROTOCOL_UNDECIDED_REQUEST,
	PROTOCOL_UNDECIDED_PING,
} protocol_undecided_sb_packet_type;

typedef struct
{
	protocol_undecided_sb_packet_type ptype;
	union
	{
		struct
		{
			int32_t protocol_version;
			/* hostname or IP, end.game. localhost or 127.0.0.1 */
			char *server_address;
			/* default is 25565 */
			uint16_t server_port;
			/* 1 for status, 2 for login, (3 for configuration) */
			int32_t next_state;
		} handshake;
		struct
		{
		} request;
		struct
		{
			/* May be any number. Notchian clients use a system-dependent time value which is counted in milliseconds. */
			int64_t payload;
		} ping;
	};
} protocol_undecided_serverbound_packet;

typedef read_result (*protocol_undecided_read_packet_fn)(bytearray *, size_t, protocol_undecided_serverbound_packet *, uint32_t *);

/* Start reading at offset. */
read_result protocol_undecided_read_packet(bytearray *data, client_state c_state, size_t offset, protocol_undecided_serverbound_packet *packet, uint32_t *bytes_read);

read_result protocol_undecided_read_handshaking_0(bytearray *data, size_t offset, protocol_undecided_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_undecided_read_status_0(bytearray *data, size_t offset, protocol_undecided_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_undecided_read_status_1(bytearray *data, size_t offset, protocol_undecided_serverbound_packet *packet, uint32_t *bytes_read);
