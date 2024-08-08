#pragma once
#include "game/event.h"
#include "net/protocol.h"
#include "types.h"
#include "util/bytearray.h"
#include <stdbool.h>

typedef struct
{
	bytearray read_buffer;
	client_state state;
} protocol_47_state;

protocol_47_state *create_state(void);
void *append_data(void *state, const uint8_t *data, size_t size);
read_result *pop_game_event(void *state, game_event *event);

typedef enum
{
	PROTOCOL_47_SB_REQUEST,
	PROTOCOL_47_SB_PING,
	PROTOCOL_47_SB_LOGIN_START,
	PROTOCOL_47_SB_ENCRYPTION_RESPONSE,
	PROTOCOL_47_SB_KEEP_ALIVE,
	PROTOCOL_47_SB_CHAT_MESSAGE,
	PROTOCOL_47_SB_USE_ENTITY,
	PROTOCOL_47_SB_PLAYER,
	PROTOCOL_47_SB_PLAYER_POSITION,
	PROTOCOL_47_SB_PLAYER_LOOK,
	PROTOCOL_47_SB_PLAYER_POSITION_AND_LOOK,
	PROTOCOL_47_SB_PLAYER_DIGGING,
	PROTOCOL_47_SB_PLAYER_BLOCK_PLACEMENT,
	PROTOCOL_47_SB_HELD_ITEM_CHANGE,
	PROTOCOL_47_SB_ANIMATION,
	PROTOCOL_47_SB_ENTITY_ACTION,
	PROTOCOL_47_SB_STEER_VEHICLE,
	PROTOCOL_47_SB_CLOSE_WINDOW,
	PROTOCOL_47_SB_CLICK_WINDOW,
	PROTOCOL_47_SB_CONFIRM_TRANSACTION,
	PROTOCOL_47_SB_CREATIVE_INVENTORY_ACTION,
	PROTOCOL_47_SB_ENCHANT_ITEM,
	PROTOCOL_47_SB_UPDATE_SIGN,
	PROTOCOL_47_SB_PLAYER_ABILITIES,
	PROTOCOL_47_SB_TAB_COMPLETE,
	PROTOCOL_47_SB_CLIENT_SETTINGS,
	PROTOCOL_47_SB_CLIENT_STATUS,
	PROTOCOL_47_SB_PLUGIN_MESSAGE,
	PROTOCOL_47_SB_SPECTATE,
	PROTOCOL_47_SB_RESOURCE_PACK_STATUS
} protocol_47_sb_packet_type;

typedef struct
{
	protocol_47_sb_packet_type ptype;
	union
	{
		struct
		{
		} request;
		struct
		{
			/* May be any number. Notchian clients use a system-dependent time value which is counted in milliseconds. */
			int64_t payload;
		} ping;
		struct
		{
			string name;
		} login_start;
		/*struct {TODO} encryption_response; */
		struct
		{
			int32_t keep_alive_id;
		} keep_alive;
		struct
		{
			/* The client sends the raw input, not Chat */
			string message;
		} chat_message;
		/*struct {TODO} use_entity; */
		struct
		{
			/* True if the client is on the ground, false otherwise */
			bool on_ground;
		} player;
		struct
		{
			/* Absolute position */
			double x;
			/* Absolute position, normally Head Y - 1.62 */
			double feet_y;
			/* Absolute position */
			double z;
			/* True if the client is on the ground, false otherwise */
			bool on_ground;
		} player_position;
		struct
		{
			/* Absolute rotation on the X Axis, in degrees */
			float yaw;
			/* Absolute rotation on the Y Axis, in degrees */
			float pitch;
			/* True if the client is on the ground, False otherwise */
			bool on_ground;
		} player_look;
		struct
		{
			/* Absolute position */
			double x;
			/* Absolute feet position, normally Head Y - 1.62 */
			double feet_y;
			/* Absolute position */
			double z;
			/* Absolute rotation on the X Axis, in degrees */
			float yaw;
			/* Absolute rotation on the Y Axis, in degrees */
			float pitch;
			/* True if the client is on the ground, false otherwise */
			bool on_ground;
		} player_position_and_look;
		struct
		{
			/* The action the player is taking against the block (see below) */
			int8_t status;
			/* Block position */
			ipos location;
			/* The face being hit (see below) */
			int8_t face;
		} player_digging;
		/*struct {TODO} player_block_placement; */
		struct
		{
			/* The slot which the player has selected (0–8) */
			int16_t slot;
		} held_item_change;
		struct
		{
		} animation;
		struct
		{
			/* Player ID */
			int32_t entity_id;
			/* The ID of the action, see below */
			int32_t action_id;
			/* Only used by Horse Jump Boost, in which case it ranges from 0 to 100. In all other cases it is 0. */
			int32_t action_parameter;
		} entity_action;
		struct
		{
			/* Positive to the left of the player */
			float sideways;
			/* Positive forward */
			float forward;
			/* Bit mask. 0x1: jump, 0x2: unmount */
			uint8_t flags;
		} steer_vehicle;
		struct
		{
			/* This is the ID of the window that was closed. 0 for player inventory. */
			uint8_t window_id;
		} close_window;
		/*struct {TODO} click_window; */
		struct
		{
			/* The ID of the window that the action occurred in */
			int8_t window_id;
			/* Every action that is to be accepted has a unique number. This field corresponds to that number. */
			int16_t action_number;
			/* Whether the action was accepted */
			bool accepted;
		} confirm_transaction;
		/*struct {TODO} creative_inventory_action; */
		struct
		{
			/* The ID of the enchantment table window sent by Open Window */
			int8_t window_id;
			/* The position of the enchantment on the enchantment table window, starting with 0 as the topmost one */
			int8_t enchantment;
		} enchant_item;
		struct
		{
			/* Block Coordinates */
			ipos location;
			/* First line of text in the sign */
			chat_obj line_1;
			/* Second line of text in the sign */
			chat_obj line_2;
			/* Third line of text in the sign */
			chat_obj line_3;
			/* Fourth line of text in the sign */
			chat_obj line_4;
		} update_sign;
		struct
		{
			/* Bit mask. 0x08: damage disabled (god mode), 0x04: can fly, 0x02: is flying, 0x01: is Creative */
			int8_t flags;
			float flying_speed;
			float walking_speed;
		} player_abilities;
		/*struct {TODO} tab_complete; */
		struct
		{
			/* e.g. en_GB */
			string locale;
			/* Client-side render distance, in chunks */
			int8_t view_distance;
			/* 0: enabled, 1: commands only, 2: hidden */
			int8_t chat_mode;
			/* “Colors” multiplayer setting */
			bool chat_colors;
			/* Skin parts, see note below */
			uint8_t displayed_skin_parts;
		} client_settings;
		struct
		{
			/* See below */
			int32_t action_id;
		} client_status;
		/*struct {TODO} plugin_message; */
		struct
		{
			/* UUID of the player to teleport to (can also be an entity UUID) */
			uuid target_player;
		} spectate;
		struct
		{
			/* The hash sent in the Resource Pack Send packet. */
			string hash;
			/* 0: successfully loaded, 1: declined, 2: failed download, 3: accepted */
			int32_t result;
		} resource_pack_status;
	} data;
} protocol_47_serverbound_packet;

read_result protocol_47_read_status_0(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_status_1(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_login_0(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_login_1(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read); /* EXT */
read_result protocol_47_read_play_0(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_1(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_2(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read); /* EXT */
read_result protocol_47_read_play_3(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_4(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_5(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_6(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_7(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_8(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read); /* EXT */
read_result protocol_47_read_play_9(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_10(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_11(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_12(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_13(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_14(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read); /* EXT */
read_result protocol_47_read_play_15(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_16(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read); /* EXT */
read_result protocol_47_read_play_17(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_18(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_19(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_20(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read); /* EXT */
read_result protocol_47_read_play_21(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_22(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_23(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read); /* EXT */
read_result protocol_47_read_play_24(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
read_result protocol_47_read_play_25(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read);
