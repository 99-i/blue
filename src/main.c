#include "cJSON.h"
#include "mem.h"
#include "net/server.h"
#include "uv.h"
#include <bits/getopt_core.h>
#include <stdio.h>
#include <stdlib.h>

static void setup_cjson_hooks(void);

int main(void)
{
	setup_cjson_hooks();
	server_settings settings;
	settings.min_version = PROTOCOL_47;
	settings.max_version = PROTOCOL_47;
	settings.online = false;

	server s;
	server_init(&s, &settings);
	server_run(&s);
	server_join(&s);

	return 0;
}

static void setup_cjson_hooks(void)
{
	cJSON_Hooks hooks;
	hooks.malloc_fn = blue_malloc;
	hooks.free_fn = blue_free;

	cJSON_InitHooks(&hooks);
}
