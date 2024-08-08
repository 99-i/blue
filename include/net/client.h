#pragma once
#include "net/protocol.h"
#include "packet/protocol_liaison.h"
#include "util/bytearray.h"
#include "uv.h"
#include <stdint.h>

typedef struct
{
	uv_tcp_t tcp_handle;
	bytearray read_buffer;
	struct s_server *s;
	protocol_version pversion;
	protocol_liaison *liaison;
} client;

void client_start(client *c);
