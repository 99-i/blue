#include "cJSON.h"
#include "memory.h"
#include "net/server.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

static void setup_cjson_hooks(void);

int main(void)
{
	setup_cjson_hooks();

	server server;
	server_init(&server);
	server_run(&server);
	server_join(&server);

	return 0;
}

static void setup_cjson_hooks(void)
{
	cJSON_Hooks hooks;
	hooks.malloc_fn = blue_malloc;
	hooks.free_fn = blue_free;

	cJSON_InitHooks(&hooks);
}
