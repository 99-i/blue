#include "game/game.h"
#include "uv.h"
#include <stdio.h>

void game_init(game *g)
{
	g->tick_count = 0;
	g->last_tick_time = uv_hrtime();
}

void game_check(game *g)
{
	uint64_t now = uv_hrtime();
	uint64_t ms_since_last_tick = (now - g->last_tick_time) / 1000000;

	if (ms_since_last_tick > 20)
	{
		printf("hi\n");
	}
}

void game_tick(game *g)
{
}
