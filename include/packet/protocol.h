#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	STATE_HANDSHAKING = 0,
	STATE_STATUS = 1,
	STATE_LOGIN = 2,
	STATE_PLAY = 3,
} client_state;

typedef enum : int
{
	PROTOCOL_UNDECIDED = 0, /* We haven't figured out the client's protocol version yet. (It comes in the handshake packet.) */
	PROTOCOL_5 = 5,			/* 1.7.10 https://wiki.vg/index.php?title=Protocol&oldid=6003 */
	PROTOCOL_47 = 47		/* 1.8.9 https://wiki.vg/index.php?title=Protocol&oldid=7368 */
} protocol_version;

/* return true if version is one that is in the protocol_version enum, e.g.
 * blue can handle it. */
bool is_protocol_version_supported(int32_t version);

const char *protocol_version_to_cstr(protocol_version version);
