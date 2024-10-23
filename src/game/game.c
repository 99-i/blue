#include "game/game.h"
#include "uv.h"
#include <bits/types/struct_timespec.h>
#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static void game_run_thread(void *data);

void game_init(game *g)
{
	g->tick_count = 0;
	g->last_tick_time = uv_hrtime();
	g->quit = false;
}

static void game_run_thread(void *data)
{
	game *g = (game *)data;
	uint64_t now;
	uint64_t ms_since_last_tick;

	while (!g->quit)
	{
		now = uv_hrtime();
		ms_since_last_tick = (now - g->last_tick_time) / 1000000;
		if (ms_since_last_tick >= 50)
		{
			game_tick(g);
			g->last_tick_time = uv_hrtime();
		}
		usleep(100);
	}
}

void game_run(game *g)
{
	uv_thread_create(&g->run_thread, game_run_thread, g);
}

void game_tick(game *g)
{
	g->tick_count++;
}

void game_schedule_quit(game *g)
{
	g->quit = true;
}

void game_join(game *g)
{
	uv_thread_join(&g->run_thread);
}
