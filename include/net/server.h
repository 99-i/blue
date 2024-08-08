#pragma once
#include "net/client.h"
#include "net/protocol.h"
#include <stdbool.h>
#include <uv.h>
#include <winsock2.h>
#include <ws2tcpip.h>

typedef struct
{
	protocol_version version;
} server_settings;

typedef struct s_server
{
	uv_tcp_t listen_handle;
	uv_thread_t run_thread;
	uv_loop_t server_loop;

	struct
	{
		size_t size;
		client **clients;
		size_t capacity;
	} clients;

	server_settings settings;
} server;

bool server_init(server *s, server_settings *settings);

void server_run(server *s);

void server_join(server *s);

void server_accept_client(server *s);

void server_client_disconnect(server *s, client *c);
