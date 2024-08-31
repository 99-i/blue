#pragma once

#include "net/client.h"
#include "types.h"
#include <uuid/uuid.h>

/* the format of this packet was changed in 735.
 * the UUID went from a string to a UUID after 735.
 * After 759, it underwent additional changes. */
void send_login_success_5to735(client *c, const char *username, uuid_t uuid);
