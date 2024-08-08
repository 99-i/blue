#pragma once

#include "util/string.h"
typedef enum
{
	CHAT_MESSAGE
} game_event_type;

typedef struct
{
	game_event_type type;
	union
	{
		string chat_message;
	};
} game_event;
