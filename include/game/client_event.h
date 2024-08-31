#pragma once

#include "packet/protocol.h"
#include <stdint.h>

typedef enum
{
	CLIENT_EVENT_HANDSHAKE,
	CLIENT_EVENT_SLP_REQUEST,
	CLIENT_EVENT_CHAT_MESSAGE
} client_event_type;

/* An event that the client sends. */
typedef struct
{
	client_event_type type;
	union
	{
		char *chat_message;
		struct
		{
			int32_t protocol_version;
			char *server_address;
			uint16_t server_port;
			int32_t next_state;
		} handshake;
	};
} client_event;
