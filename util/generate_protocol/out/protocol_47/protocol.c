#include "packet/protocol_47/protocol.h"
#include "mem.h"
#include "packet/read_fn.h"

protocol_47_state *protocol_47_create_state(void)
{
	protocol_47_state *state = blue_malloc(sizeof(*state));
	return state;
}

static read_result protocol_47_pop_packet(bytearray *read_buffer, client_state c_state, void *state, protocol_47_serverbound_packet *packet);

read_result protocol_47_pop_client_event(void *state, client_state c_state, bytearray *read_buffer, client_event *event)
{
	read_result result;
	protocol_47_serverbound_packet packet;
	uint32_t bytes_read;

	/* Attempt to pop packet. */
	result = protocol_47_pop_packet(read_buffer, c_state, state, &packet);

	if(result) return result;

	/* Convert to client event.*/

}
static read_result protocol_47_pop_packet(bytearray *read_buffer, client_state c_state, void *state, protocol_47_serverbound_packet *packet)
{
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

	printf("Read packet with length %d\n", packet_length);

	/* construct temporary buffer with size of packet length. */

	temporary_packet_buffer.capacity = packet_length;
	temporary_packet_buffer.size = packet_length;
	temporary_packet_buffer.data = read_buffer->data;

	result = protocol_47_read_packet(&temporary_packet_buffer, c_state, size, packet, &size_buffer);
	if(result) return result;
	if((int32_t) size_buffer != packet_length) {
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
	static const protocol_47_read_packet_fn read_status[2] = {protocol_47_read_status_0, protocol_47_read_status_1};
static const protocol_47_read_packet_fn read_login[2] = {protocol_47_read_login_0, protocol_47_read_login_1};
static const protocol_47_read_packet_fn read_play[26] = {protocol_47_read_play_0, protocol_47_read_play_1, protocol_47_read_play_2, protocol_47_read_play_3, protocol_47_read_play_4, protocol_47_read_play_5, protocol_47_read_play_6, protocol_47_read_play_7, protocol_47_read_play_8, protocol_47_read_play_9, protocol_47_read_play_10, protocol_47_read_play_11, protocol_47_read_play_12, protocol_47_read_play_13, protocol_47_read_play_14, protocol_47_read_play_15, protocol_47_read_play_16, protocol_47_read_play_17, protocol_47_read_play_18, protocol_47_read_play_19, protocol_47_read_play_20, protocol_47_read_play_21, protocol_47_read_play_22, protocol_47_read_play_23, protocol_47_read_play_24, protocol_47_read_play_25};


	result = read_varint(data, offset, &id, &size);
	if (!result)
		return result;

	offset += size;

	switch (c_state)
	{
		case STATE_STATUS:
			return read_status[id](data, offset, packet, &size);
		case STATE_LOGIN:
			return read_login[id](data, offset, packet, &size);
		case STATE_PLAY:
			return read_play[id](data, offset, packet, &size);
		case STATE_HANDSHAKING:
			return READ_RESULT_MALFORMED;
	}
	return READ_RESULT_MALFORMED;
}

#define DO_READ(read_fn, variable_name)                                        	result = read_fn(data, offset + bytes_read_buffer, &variable_name, &size); 	if (result)                                                                		return result;                                                         	bytes_read_buffer += size;

#define DO_READ_BUFFER(buffer_size, variable_name)                            	result = read_bytearray(data, offset + bytes_read_buffer, &variable_name, 							buffer_size, &size);                              	if (result)                                                               		return result;                                                        	bytes_read_buffer += size;

	read_result protocol_47_read_status_0(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	


	packet->ptype = PROTOCOL_47_SB_REQUEST;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_status_1(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	
	int64_t payload;

	DO_READ(read_i64, payload);

	packet->ptype = PROTOCOL_47_SB_PING;
	packet->data.ping.payload = payload;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_login_0(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	
	string name;

	DO_READ(read_string, name);

	packet->ptype = PROTOCOL_47_SB_LOGIN_START;
	packet->data.login_start.name = name;

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
	packet->data.keep_alive.keep_alive_id = keep_alive_id;

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
	
	string message;

	DO_READ(read_string, message);

	packet->ptype = PROTOCOL_47_SB_CHAT_MESSAGE;
	packet->data.chat_message.message = message;

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
	packet->data.player.on_ground = on_ground;

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
	packet->data.player_position.x = x;
	packet->data.player_position.feet_y = feet_y;
	packet->data.player_position.z = z;
	packet->data.player_position.on_ground = on_ground;

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
	packet->data.player_look.yaw = yaw;
	packet->data.player_look.pitch = pitch;
	packet->data.player_look.on_ground = on_ground;

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
	packet->data.player_position_and_look.x = x;
	packet->data.player_position_and_look.feet_y = feet_y;
	packet->data.player_position_and_look.z = z;
	packet->data.player_position_and_look.yaw = yaw;
	packet->data.player_position_and_look.pitch = pitch;
	packet->data.player_position_and_look.on_ground = on_ground;

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
	packet->data.player_digging.status = status;
	packet->data.player_digging.location = location;
	packet->data.player_digging.face = face;

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
	packet->data.held_item_change.slot = slot;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
read_result protocol_47_read_play_10(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	


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
	packet->data.entity_action.entity_id = entity_id;
	packet->data.entity_action.action_id = action_id;
	packet->data.entity_action.action_parameter = action_parameter;

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
	packet->data.steer_vehicle.sideways = sideways;
	packet->data.steer_vehicle.forward = forward;
	packet->data.steer_vehicle.flags = flags;

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
	packet->data.close_window.window_id = window_id;

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
	packet->data.confirm_transaction.window_id = window_id;
	packet->data.confirm_transaction.action_number = action_number;
	packet->data.confirm_transaction.accepted = accepted;

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
	packet->data.enchant_item.window_id = window_id;
	packet->data.enchant_item.enchantment = enchantment;

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
	packet->data.update_sign.location = location;
	packet->data.update_sign.line_1 = line_1;
	packet->data.update_sign.line_2 = line_2;
	packet->data.update_sign.line_3 = line_3;
	packet->data.update_sign.line_4 = line_4;

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
	packet->data.player_abilities.flags = flags;
	packet->data.player_abilities.flying_speed = flying_speed;
	packet->data.player_abilities.walking_speed = walking_speed;

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
	
	string locale;
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
	packet->data.client_settings.locale = locale;
	packet->data.client_settings.view_distance = view_distance;
	packet->data.client_settings.chat_mode = chat_mode;
	packet->data.client_settings.chat_colors = chat_colors;
	packet->data.client_settings.displayed_skin_parts = displayed_skin_parts;

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
	packet->data.client_status.action_id = action_id;

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
	
	uuid target_player;

	DO_READ(read_uuid, target_player);

	packet->ptype = PROTOCOL_47_SB_SPECTATE;
	packet->data.spectate.target_player = target_player;

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
	
	string hash;
	int32_t result;

	DO_READ(read_string, hash);
	DO_READ(read_varint, result);

	packet->ptype = PROTOCOL_47_SB_RESOURCE_PACK_STATUS;
	packet->data.resource_pack_status.hash = hash;
	packet->data.resource_pack_status.result = result;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

