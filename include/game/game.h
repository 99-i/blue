#pragma once
#include "uv.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

typedef struct
{
	uint64_t tick_count;
	uint64_t last_tick_time;
	uv_thread_t run_thread;
	bool quit;
} game;

void game_init(game *g);

/* runs the game in a separate thread. */
void game_run(game *g);

void game_tick(game *g);

void game_schedule_quit(game *g);

void game_join(game *g);
