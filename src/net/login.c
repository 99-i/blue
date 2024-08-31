#include "net/login.h"
#include "net/client.h"
#include "packet/packet_builder.h"
#include <uuid/uuid.h>

void send_login_success_5to735(client *c, const char *username, uuid_t uuid)
{
	packet_builder builder;
	char uuid_str[37] = {0};

	packet_build(&builder, 2);

	uuid_unparse(uuid, uuid_str);

	printf("UUID string: %s", uuid_str);

	packet_append_string(&builder, uuid_str);
	packet_append_string(&builder, username);

	packet_package(&builder);

	client_send_raw_bytearray(c, &builder.buffer);

	packet_free_builder(&builder);
}
