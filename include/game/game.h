#pragma once
#include <stdint.h>
#include <sys/time.h>

typedef struct
{
	uint64_t tick_count;
	uint64_t last_tick_time;
} game;

void game_init(game *g);

/* this function is called on every uv_timer_cb, NOT every 50ms.
 * it's up to the game to handle that */
void game_check(game *g);

void game_tick(game *g);
