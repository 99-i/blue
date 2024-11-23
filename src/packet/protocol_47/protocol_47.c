#include "packet/protocol_47.h"
#include "game/client_event.h"
#include "log.h"
#include "mem.h"
#include "net/client.h"
#include "net/login.h"
#include "net/server.h"
#include "packet/read_fn.h"
#include "types.h"
#include "util/bytearray.h"
#include <assert.h>
#include <string.h>

protocol_47_state *protocol_47_create_state(void)
{
	protocol_47_state *state = blue_malloc(sizeof(*state));
	return state;
}

static read_result protocol_47_pop_packet(bytearray *read_buffer, client_state c_state, protocol_47_state *state, protocol_47_serverbound_packet *packet);

/* only free the members, not the packet object itself. */
static void free_packet(protocol_47_serverbound_packet *packet);

read_result protocol_47_pop_client_event(liaison *l, client_event *event)
{
	read_result result;
	protocol_47_serverbound_packet packet;

	/* Attempt to pop packet. */
	result = protocol_47_pop_packet(&l->read_buffer, l->c_state, l->liaison_state, &packet);

	if (result)
		return result;

	/* Convert to client event. TODO */

	log_trace("Got P47 packet with type %d", packet.ptype);
	switch (packet.ptype)
	{
		case PROTOCOL_47_SB_LOGIN_START:
			log_trace("Received login start packet with name %s.", packet.login_start.name);
			if (l->c->s->settings.online)
			{
				/* TODO: Online mode. */
			}
			else
			{
				uuid_t uuid = {0};

				send_login_success_5to735(l->c, packet.login_start.name, uuid);
			}
			break;
	}

	free_packet(&packet);

	event->type = CLIENT_EVENT_CHAT_MESSAGE;

	return READ_RESULT_SUCCESS;
}

static void free_packet(protocol_47_serverbound_packet *packet)
{
	switch (packet->ptype)
	{
		case PROTOCOL_47_SB_LOGIN_START:
			blue_free(packet->login_start.name);
			break;
	}
}

static read_result protocol_47_pop_packet(bytearray *read_buffer, client_state c_state, protocol_47_state *state, protocol_47_serverbound_packet *packet)
{
	(void)state;
	uint32_t size;
	uint32_t size_buffer;
	int32_t packet_length;
	read_result result;
	bytearray temporary_packet_buffer;

	result = read_varint(read_buffer, 0, &packet_length, &size);
	if (result)
		return result;

	if (read_buffer->size - size < (size_t)packet_length)
	{
		return READ_RESULT_NOT_ENOUGH_DATA;
	}

	/* construct temporary buffer with size of packet length. */

	temporary_packet_buffer.capacity = packet_length;
	temporary_packet_buffer.size = packet_length;
	temporary_packet_buffer.data = read_buffer->data + size;

	result = protocol_47_read_packet(&temporary_packet_buffer, c_state, 0, packet, &size_buffer);
	if (result)
		return result;

	if ((int32_t)size_buffer != packet_length)
	{
		return READ_RESULT_MALFORMED;
	}

	bytearray_slice_from_beginning(read_buffer, size + size_buffer);

	return READ_RESULT_SUCCESS;
}

read_result protocol_47_read_packet(bytearray *data, client_state c_state, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	int32_t id;
	uint32_t size;
	read_result result;
	static const protocol_47_read_packet_fn read_login[] = {protocol_47_read_login_0, protocol_47_read_login_1};
	static const protocol_47_read_packet_fn read_play[] = {
		protocol_47_read_play_0,
		protocol_47_read_play_1,
		protocol_47_read_play_2,
		protocol_47_read_play_3,
		protocol_47_read_play_4,
		protocol_47_read_play_5,
		protocol_47_read_play_6,
		protocol_47_read_play_7,
		protocol_47_read_play_8,
		protocol_47_read_play_9,
		protocol_47_read_play_10,
		protocol_47_read_play_11,
		protocol_47_read_play_12,
		protocol_47_read_play_13,
		protocol_47_read_play_14,
		protocol_47_read_play_15,
		protocol_47_read_play_16,
		protocol_47_read_play_17,
		protocol_47_read_play_18,
		protocol_47_read_play_19,
		protocol_47_read_play_20,
		protocol_47_read_play_21,
		protocol_47_read_play_22,
		protocol_47_read_play_23,
		protocol_47_read_play_24,
		protocol_47_read_play_25,
	};

	result = read_varint(data, offset, &id, &size);

	if (result)
		return result;

	offset += size;

	switch (c_state)
	{
		case STATE_LOGIN:
			if ((unsigned long)id >= sizeof(read_login) / sizeof(read_login[0]))
			{
				return READ_RESULT_MALFORMED;
			}
			result = read_login[id](data, offset, packet, &size);
			break;
		case STATE_PLAY:
			if ((unsigned long)id >= sizeof(read_play) / sizeof(read_play[0]))
			{
				return READ_RESULT_MALFORMED;
			}
			result = read_play[id](data, offset, packet, &size);
			break;
		default:
			assert(false && "how");
			return READ_RESULT_MALFORMED;
	}

	if (result)
		return result;

	offset += size;

	if (bytes_read)
	{
		*bytes_read = offset;
	}

	return READ_RESULT_SUCCESS;
}

read_result protocol_47_read_login_0(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	char *name;

	DO_READ(read_string, name);

	packet->ptype = PROTOCOL_47_SB_LOGIN_START;
	packet->login_start.name = name;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_0(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int32_t keep_alive_id;

	DO_READ(read_varint, keep_alive_id);

	packet->ptype = PROTOCOL_47_SB_KEEP_ALIVE;
	packet->keep_alive.keep_alive_id = keep_alive_id;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_1(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	char *message;

	DO_READ(read_string, message);

	packet->ptype = PROTOCOL_47_SB_CHAT_MESSAGE;
	packet->chat_message.message = message;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_3(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	bool on_ground;

	DO_READ(read_bool, on_ground);

	packet->ptype = PROTOCOL_47_SB_PLAYER;
	packet->player.on_ground = on_ground;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_4(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	double x;
	double feet_y;
	double z;
	bool on_ground;

	DO_READ(read_double, x);
	DO_READ(read_double, feet_y);
	DO_READ(read_double, z);
	DO_READ(read_bool, on_ground);

	packet->ptype = PROTOCOL_47_SB_PLAYER_POSITION;
	packet->player_position.x = x;
	packet->player_position.feet_y = feet_y;
	packet->player_position.z = z;
	packet->player_position.on_ground = on_ground;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_5(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	float yaw;
	float pitch;
	bool on_ground;

	DO_READ(read_float, yaw);
	DO_READ(read_float, pitch);
	DO_READ(read_bool, on_ground);

	packet->ptype = PROTOCOL_47_SB_PLAYER_LOOK;
	packet->player_look.yaw = yaw;
	packet->player_look.pitch = pitch;
	packet->player_look.on_ground = on_ground;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_6(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	double x;
	double feet_y;
	double z;
	float yaw;
	float pitch;
	bool on_ground;

	DO_READ(read_double, x);
	DO_READ(read_double, feet_y);
	DO_READ(read_double, z);
	DO_READ(read_float, yaw);
	DO_READ(read_float, pitch);
	DO_READ(read_bool, on_ground);

	packet->ptype = PROTOCOL_47_SB_PLAYER_POSITION_AND_LOOK;
	packet->player_position_and_look.x = x;
	packet->player_position_and_look.feet_y = feet_y;
	packet->player_position_and_look.z = z;
	packet->player_position_and_look.yaw = yaw;
	packet->player_position_and_look.pitch = pitch;
	packet->player_position_and_look.on_ground = on_ground;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_7(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int8_t status;
	ipos location;
	int8_t face;

	DO_READ(read_i8, status);
	DO_READ(read_position, location);
	DO_READ(read_i8, face);

	packet->ptype = PROTOCOL_47_SB_PLAYER_DIGGING;
	packet->player_digging.status = status;
	packet->player_digging.location = location;
	packet->player_digging.face = face;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_9(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int16_t slot;

	DO_READ(read_i16, slot);

	packet->ptype = PROTOCOL_47_SB_HELD_ITEM_CHANGE;
	packet->held_item_change.slot = slot;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_10(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	(void)data;
	(void)offset;
	uint32_t bytes_read_buffer = 0;

	packet->ptype = PROTOCOL_47_SB_ANIMATION;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_11(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int32_t entity_id;
	int32_t action_id;
	int32_t action_parameter;

	DO_READ(read_varint, entity_id);
	DO_READ(read_varint, action_id);
	DO_READ(read_varint, action_parameter);

	packet->ptype = PROTOCOL_47_SB_ENTITY_ACTION;
	packet->entity_action.entity_id = entity_id;
	packet->entity_action.action_id = action_id;
	packet->entity_action.action_parameter = action_parameter;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_12(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	float sideways;
	float forward;
	uint8_t flags;

	DO_READ(read_float, sideways);
	DO_READ(read_float, forward);
	DO_READ(read_u8, flags);

	packet->ptype = PROTOCOL_47_SB_STEER_VEHICLE;
	packet->steer_vehicle.sideways = sideways;
	packet->steer_vehicle.forward = forward;
	packet->steer_vehicle.flags = flags;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_13(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	uint8_t window_id;

	DO_READ(read_u8, window_id);

	packet->ptype = PROTOCOL_47_SB_CLOSE_WINDOW;
	packet->close_window.window_id = window_id;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_15(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int8_t window_id;
	int16_t action_number;
	bool accepted;

	DO_READ(read_i8, window_id);
	DO_READ(read_i16, action_number);
	DO_READ(read_bool, accepted);

	packet->ptype = PROTOCOL_47_SB_CONFIRM_TRANSACTION;
	packet->confirm_transaction.window_id = window_id;
	packet->confirm_transaction.action_number = action_number;
	packet->confirm_transaction.accepted = accepted;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_17(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int8_t window_id;
	int8_t enchantment;

	DO_READ(read_i8, window_id);
	DO_READ(read_i8, enchantment);

	packet->ptype = PROTOCOL_47_SB_ENCHANT_ITEM;
	packet->enchant_item.window_id = window_id;
	packet->enchant_item.enchantment = enchantment;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_18(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	ipos location;
	chat_obj line_1;
	chat_obj line_2;
	chat_obj line_3;
	chat_obj line_4;

	DO_READ(read_position, location);
	DO_READ(read_chat, line_1);
	DO_READ(read_chat, line_2);
	DO_READ(read_chat, line_3);
	DO_READ(read_chat, line_4);

	packet->ptype = PROTOCOL_47_SB_UPDATE_SIGN;
	packet->update_sign.location = location;
	packet->update_sign.line_1 = line_1;
	packet->update_sign.line_2 = line_2;
	packet->update_sign.line_3 = line_3;
	packet->update_sign.line_4 = line_4;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_19(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int8_t flags;
	float flying_speed;
	float walking_speed;

	DO_READ(read_i8, flags);
	DO_READ(read_float, flying_speed);
	DO_READ(read_float, walking_speed);

	packet->ptype = PROTOCOL_47_SB_PLAYER_ABILITIES;
	packet->player_abilities.flags = flags;
	packet->player_abilities.flying_speed = flying_speed;
	packet->player_abilities.walking_speed = walking_speed;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_21(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	char *locale;
	int8_t view_distance;
	int8_t chat_mode;
	bool chat_colors;
	uint8_t displayed_skin_parts;

	DO_READ(read_string, locale);
	DO_READ(read_i8, view_distance);
	DO_READ(read_i8, chat_mode);
	DO_READ(read_bool, chat_colors);
	DO_READ(read_u8, displayed_skin_parts);

	packet->ptype = PROTOCOL_47_SB_CLIENT_SETTINGS;
	packet->client_settings.locale = locale;
	packet->client_settings.view_distance = view_distance;
	packet->client_settings.chat_mode = chat_mode;
	packet->client_settings.chat_colors = chat_colors;
	packet->client_settings.displayed_skin_parts = displayed_skin_parts;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_22(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int32_t action_id;

	DO_READ(read_varint, action_id);

	packet->ptype = PROTOCOL_47_SB_CLIENT_STATUS;
	packet->client_status.action_id = action_id;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_24(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	uuid_t target_player;

	DO_READ(read_uuid, target_player);

	packet->ptype = PROTOCOL_47_SB_SPECTATE;

	memcpy(&packet->spectate.target_player, &target_player, sizeof(uuid_t));

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_25(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	char *hash;
	int32_t rezult;

	DO_READ(read_string, hash);
	DO_READ(read_varint, rezult);

	packet->ptype = PROTOCOL_47_SB_RESOURCE_PACK_STATUS;
	packet->resource_pack_status.hash = hash;
	packet->resource_pack_status.result = rezult;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
