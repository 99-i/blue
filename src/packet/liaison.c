#include "packet/liaison.h"
#include "mem.h"
#include "net/client.h"
#include "net/server.h"
#include "packet/protocol.h"
#include "packet/protocol_47.h"
#include "packet/protocol_undecided.h"
#include "packet/read_fn.h"
#include "types.h"
#include "util/bytearray.h"
#include <assert.h>
#include <locale.h>
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

void liaison_free(liaison *l)
{
	blue_free(l->liaison_state);
	blue_free(l->read_buffer.data);
	blue_free(l);
}
