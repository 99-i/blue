#pragma once

/* tn event that happens in the game. sent to clients. */
/* track these for replay technology. */
typedef enum
{
	PLAYER_JOIN,
	ENTITY_MOVE
} game_event_type;

typedef struct
{
	game_event_type type;

	union
	{
		struct
		{
		} player_join;
		struct
		{
		} entity_move;
	};

} game_event;
