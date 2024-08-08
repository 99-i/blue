#include "packet/protocol_47/protocol.h"
#include "memory.h"
#include "packet/read_fn.h"

static read_result read_packet(client_state state, bytearray *data, size_t offset, serverbound_47_packet *packet);

typedef read_result (*read_fn)(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Handshake */
static read_result read_handshaking_0(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Request */
static read_result read_status_0(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Ping */
static read_result read_status_1(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Login Start */
static read_result read_login_0(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Encryption Response */
static read_result read_login_1(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Keep Alive */
static read_result read_play_0(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Chat Message */
static read_result read_play_1(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Use Entity */
static read_result read_play_2(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Player */
static read_result read_play_3(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Player Position */
static read_result read_play_4(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Player Look */
static read_result read_play_5(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Player Position And Look */
static read_result read_play_6(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Player Digging */
static read_result read_play_7(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Player Block Placement */
static read_result read_play_8(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Held Item Change */
static read_result read_play_9(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Animation */
static read_result read_play_10(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Entity Action */
static read_result read_play_11(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Steer Vehicle */
static read_result read_play_12(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Close Window */
static read_result read_play_13(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Click Window */
static read_result read_play_14(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Confirm Transaction */
static read_result read_play_15(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Creative Inventory Action */
static read_result read_play_16(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Enchant Item */
static read_result read_play_17(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Update Sign */
static read_result read_play_18(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Player Abilities */
static read_result read_play_19(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Tab-Complete */
static read_result read_play_20(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Client Settings */
static read_result read_play_21(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Client Status */
static read_result read_play_22(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Plugin Message */
static read_result read_play_23(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Spectate */
static read_result read_play_24(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

/* Resource Pack Status */
static read_result read_play_25(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read);

protocol_47_state *protocol_47_create_state(void)
{
	protocol_47_state *state = blue_malloc(sizeof(protocol_47_state));

	state->state = STATE_HANDSHAKING;

	bytearray_init(&state->read_buffer);

	return state;
}

void protocol_47_append_data(void *state, const uint8_t *data, size_t size)
{
	protocol_47_state *pstate = state;

	bytearray_append_data(&pstate->read_buffer, data, size);
}

read_result packet_pop(client_state state, bytearray *data, serverbound_47_packet *packet);

/* attempt to pop a packet
 * if malformed, return READ_RESULT_MALFORMED
 * if not enough data, return READ_RESULT_NOT_ENOUGH_DATA */
read_result protocol_47_pop_game_event(void *state, game_event *event)
{
	protocol_47_state *pstate = state;
	serverbound_47_packet packet;
	read_result result;

	/* first, read the packet. */
	result = packet_pop(pstate->state, &pstate->read_buffer, &packet);
	if (result)
		return result;

	/* next, convert it to a game event. */

	switch (packet.ptype)
	{
		case SBPTYPE_REQUEST:
			break;
		case SBPTYPE_PING:
			break;
		case SBPTYPE_LOGIN_START:
			break;
		case SBPTYPE_ENCRYPTION_RESPONSE:
			break;
		case SBPTYPE_KEEP_ALIVE:
			break;
		case SBPTYPE_CHAT_MESSAGE:
			break;
		case SBPTYPE_USE_ENTITY:
			break;
		case SBPTYPE_PLAYER:
			break;
		case SBPTYPE_PLAYER_POSITION:
			break;
		case SBPTYPE_PLAYER_LOOK:
			break;
		case SBPTYPE_PLAYER_POSITION_AND_LOOK:
			break;
		case SBPTYPE_PLAYER_DIGGING:
			break;
		case SBPTYPE_PLAYER_BLOCK_PLACEMENT:
			break;
		case SBPTYPE_HELD_ITEM_CHANGE:
			break;
		case SBPTYPE_ANIMATION:
			break;
		case SBPTYPE_ENTITY_ACTION:
			break;
		case SBPTYPE_STEER_VEHICLE:
			break;
		case SBPTYPE_CLOSE_WINDOW:
			break;
		case SBPTYPE_CLICK_WINDOW:
			break;
		case SBPTYPE_CONFIRM_TRANSACTION:
			break;
		case SBPTYPE_CREATIVE_INVENTORY_ACTION:
			break;
		case SBPTYPE_ENCHANT_ITEM:
			break;
		case SBPTYPE_UPDATE_SIGN:
			break;
		case SBPTYPE_PLAYER_ABILITIES:
			break;
		case SBPTYPE_TAB_COMPLETE:
			break;
		case SBPTYPE_CLIENT_SETTINGS:
			break;
		case SBPTYPE_CLIENT_STATUS:
			break;
		case SBPTYPE_PLUGIN_MESSAGE:
			break;
		case SBPTYPE_SPECTATE:
			break;
		case SBPTYPE_RESOURCE_PACK_STATUS:
			break;
	}
}

read_result packet_pop(client_state state, bytearray *data, serverbound_47_packet *packet)
{
	uint32_t size;
	int32_t packet_length;
	read_result result;
	bytearray temporary_packet_buffer;

	result = read_varint(data, 0, &packet_length, &size);
	if (!result)
		return result;

	if (data->size - size < (size_t)packet_length)
	{
		return READ_RESULT_NOT_ENOUGH_DATA;
	}

	printf("Read packet with length %d\n", packet_length);

	/* construct temporary buffer with capacity of packet length. */

	temporary_packet_buffer.capacity = packet_length;
	temporary_packet_buffer.size = packet_length;
	temporary_packet_buffer.data = data->data;

	return read_packet(state, &temporary_packet_buffer, size, packet);
}

static read_result read_packet(client_state state, bytearray *data, size_t offset, serverbound_47_packet *packet)
{
	int32_t id;
	uint32_t size;
	read_result result;

	static const read_fn read_handshaking[1] = {read_handshaking_0};
	static const read_fn read_status[2] = {read_status_0, read_status_1};
	static const read_fn read_login[2] = {read_login_0, read_login_1};
	static const read_fn read_play[26] = {
		read_play_0,
		read_play_1,
		read_play_2,
		read_play_3,
		read_play_4,
		read_play_5,
		read_play_6,
		read_play_7,
		read_play_8,
		read_play_9,
		read_play_10,
		read_play_11,
		read_play_12,
		read_play_13,
		read_play_14,
		read_play_15,
		read_play_16,
		read_play_17,
		read_play_18,
		read_play_19,
		read_play_20,
		read_play_21,
		read_play_22,
		read_play_23,
		read_play_24,
		read_play_25,
	};

	result = read_varint(data, offset, &id, &size);
	if (!result)
		return result;

	offset += size;

	switch (state)
	{
		case STATE_HANDSHAKING:
			return read_handshaking[id](data, offset, packet, &size);
		case STATE_STATUS:
			return read_status[id](data, offset, packet, &size);
		case STATE_LOGIN:
			return read_login[id](data, offset, packet, &size);
		case STATE_PLAY:
			return read_play[id](data, offset, packet, &size);
	}
	return READ_RESULT_MALFORMED;
}

#define DO_READ(read_fn, variable_name)                                        \
	result = read_fn(data, offset + bytes_read_buffer, &variable_name, &size); \
	if (!result)                                                               \
		return result;                                                         \
	bytes_read_buffer += size;

#define DO_READ_BUFFER(buffer_size, variable_name)                            \
	result = read_bytearray(data, offset + bytes_read_buffer, &variable_name, \
							buffer_size, &size);                              \
	if (!result)                                                              \
		return result;                                                        \
	bytes_read_buffer += size;

static read_result read_handshaking_0(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int32_t protocol_version;
	string server_address;
	uint16_t server_port;
	int32_t next_state;

	DO_READ(read_varint, protocol_version);
	DO_READ(read_string, server_address);
	DO_READ(read_u16, server_port);
	DO_READ(read_varint, next_state);
	packet->ptype = SBPTYPE_HANDSHAKE;
	packet->data.handshake.protocol_version = protocol_version;
	packet->data.handshake.server_address = server_address;
	packet->data.handshake.server_port = server_port;
	packet->data.handshake.next_state = next_state;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_status_0(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	(void)data;
	(void)offset;
	packet->ptype = SBPTYPE_REQUEST;
	if (bytes_read)
	{
		*bytes_read = 0;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_status_1(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int64_t payload;

	DO_READ(read_i64, payload);
	packet->ptype = SBPTYPE_PING;
	packet->data.ping.payload = payload;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_login_0(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	string name;

	DO_READ(read_string, name);
	packet->ptype = SBPTYPE_LOGIN_START;
	packet->data.login_start.name = name;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_login_1(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int32_t shared_secret_length;
	bytearray shared_secret;
	int32_t verify_token_length;
	bytearray verify_token;

	DO_READ(read_varint, shared_secret_length);
	DO_READ_BUFFER(shared_secret_length, shared_secret);
	DO_READ(read_varint, verify_token_length);
	DO_READ_BUFFER(verify_token_length, verify_token);

	packet->ptype = SBPTYPE_ENCRYPTION_RESPONSE;
	packet->data.encryption_response.shared_secret_length =
		shared_secret_length;
	packet->data.encryption_response.shared_secret = shared_secret;
	packet->data.encryption_response.verify_token_length = verify_token_length;
	packet->data.encryption_response.verify_token = verify_token;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_0(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int32_t keep_alive_id;

	DO_READ(read_varint, keep_alive_id);
	packet->ptype = SBPTYPE_KEEP_ALIVE;
	packet->data.keep_alive.keep_alive_id = keep_alive_id;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_1(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	string message;

	DO_READ(read_string, message);
	packet->ptype = SBPTYPE_CHAT_MESSAGE;
	packet->data.chat_message.message = message;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_2(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int32_t target;
	int32_t type;
	float target_x = 0;
	float target_y = 0;
	float target_z = 0;

	DO_READ(read_varint, target);
	DO_READ(read_varint, type);
	if (type == INTERACT_AT)
	{
		DO_READ(read_float, target_x);
		DO_READ(read_float, target_y);
		DO_READ(read_float, target_z);
	}

	packet->ptype = SBPTYPE_USE_ENTITY;
	packet->data.use_entity.target = target;
	packet->data.use_entity.type = type;
	packet->data.use_entity.target_x = target_x;
	packet->data.use_entity.target_y = target_y;
	packet->data.use_entity.target_z = target_z;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_3(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	bool on_ground;

	DO_READ(read_bool, on_ground);
	packet->ptype = SBPTYPE_PLAYER;
	packet->data.player.on_ground = on_ground;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_4(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
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
	packet->ptype = SBPTYPE_PLAYER_POSITION;
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

static read_result read_play_5(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
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
	packet->ptype = SBPTYPE_PLAYER_LOOK;
	packet->data.player_look.yaw = yaw;
	packet->data.player_look.pitch = pitch;
	packet->data.player_look.on_ground = on_ground;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_6(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
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
	packet->ptype = SBPTYPE_PLAYER_POSITION_AND_LOOK;
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

static read_result read_play_7(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
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
	packet->ptype = SBPTYPE_PLAYER_DIGGING;
	packet->data.player_digging.status = status;
	packet->data.player_digging.location = location;
	packet->data.player_digging.face = face;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_8(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	ipos location;
	int8_t face;
	/*	<SKIP> held_item;
	int8_t cursor_position_x;
	int8_t cursor_position_y;
	int8_t cursor_position_z;

	DO_READ(read_position, location);
	DO_READ(read_i8, face);
	DO_READ(<READFNMISSING>, held_item);
	DO_READ(read_i8, cursor_position_x);
	DO_READ(read_i8, cursor_position_y);
	DO_READ(read_i8, cursor_position_z);
	packet->ptype = SBPTYPE_PLAYER_BLOCK_PLACEMENT;
	packet->data.player_block_placement.location = location;
	packet->data.player_block_placement.face = face;
	packet->data.player_block_placement.held_item = held_item;
	packet->data.player_block_placement.cursor_position_x = cursor_position_x;
	packet->data.player_block_placement.cursor_position_y = cursor_position_y;
	packet->data.player_block_placement.cursor_position_z = cursor_position_z;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;*/
}

static read_result read_play_9(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int16_t slot;

	DO_READ(read_i16, slot);
	packet->ptype = SBPTYPE_HELD_ITEM_CHANGE;
	packet->data.held_item_change.slot = slot;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_10(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	(void)data;
	(void)offset;
	packet->ptype = SBPTYPE_ANIMATION;
	if (bytes_read)
	{
		*bytes_read = 0;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_11(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
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
	packet->ptype = SBPTYPE_ENTITY_ACTION;
	packet->data.entity_action.entity_id = entity_id;
	packet->data.entity_action.action_id = action_id;
	packet->data.entity_action.action_parameter = action_parameter;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_12(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
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
	packet->ptype = SBPTYPE_STEER_VEHICLE;
	packet->data.steer_vehicle.sideways = sideways;
	packet->data.steer_vehicle.forward = forward;
	packet->data.steer_vehicle.flags = flags;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_13(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	uint8_t window_id;

	DO_READ(read_u8, window_id);
	packet->ptype = SBPTYPE_CLOSE_WINDOW;
	packet->data.close_window.window_id = window_id;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_14(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	uint8_t window_id;
	int16_t slot;
	int8_t button;
	int16_t action_number;
	int8_t mode;
	/*<SKIP> clicked_item;

	DO_READ(read_u8, window_id);
	DO_READ(read_i16, slot);
	DO_READ(read_i8, button);
	DO_READ(read_i16, action_number);
	DO_READ(read_i8<ENUM>, mode);
	DO_READ(<READFNMISSING>, clicked_item);
	packet->ptype = SBPTYPE_CLICK_WINDOW;
	packet->data.click_window.window_id = window_id;
	packet->data.click_window.slot = slot;
	packet->data.click_window.button = button;
	packet->data.click_window.action_number = action_number;
	packet->data.click_window.mode = mode;
	packet->data.click_window.clicked_item = clicked_item;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;*/
}

static read_result read_play_15(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
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
	packet->ptype = SBPTYPE_CONFIRM_TRANSACTION;
	packet->data.confirm_transaction.window_id = window_id;
	packet->data.confirm_transaction.action_number = action_number;
	packet->data.confirm_transaction.accepted = accepted;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_16(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int16_t slot;
	/*<SKIP> clicked_item;

	DO_READ(read_i16, slot);
	DO_READ(<READFNMISSING>, clicked_item);
	packet->ptype = SBPTYPE_CREATIVE_INVENTORY_ACTION;
	packet->data.creative_inventory_action.slot = slot;
	packet->data.creative_inventory_action.clicked_item = clicked_item;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;*/
}

static read_result read_play_17(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int8_t window_id;
	int8_t enchantment;

	DO_READ(read_i8, window_id);
	DO_READ(read_i8, enchantment);
	packet->ptype = SBPTYPE_ENCHANT_ITEM;
	packet->data.enchant_item.window_id = window_id;
	packet->data.enchant_item.enchantment = enchantment;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_18(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
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
	packet->ptype = SBPTYPE_UPDATE_SIGN;
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

static read_result read_play_19(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
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
	packet->ptype = SBPTYPE_PLAYER_ABILITIES;
	packet->data.player_abilities.flags = flags;
	packet->data.player_abilities.flying_speed = flying_speed;
	packet->data.player_abilities.walking_speed = walking_speed;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_20(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	string text;
	bool has_position;
	ipos looked_at_block;

	DO_READ(read_string, text);
	DO_READ(read_bool, has_position);
	DO_READ(read_position, looked_at_block);
	packet->ptype = SBPTYPE_TAB_COMPLETE;
	packet->data.tab_complete.text = text;
	packet->data.tab_complete.has_position = has_position;
	packet->data.tab_complete.looked_at_block = looked_at_block;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_21(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
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
	packet->ptype = SBPTYPE_CLIENT_SETTINGS;
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

static read_result read_play_22(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	int32_t action_id;

	DO_READ(read_varint, action_id);
	packet->ptype = SBPTYPE_CLIENT_STATUS;
	packet->data.client_status.action_id = action_id;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_23(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	string channel;
	bytearray plugin_message_data;

	DO_READ(read_string, channel);
	DO_READ_BUFFER(data->size - offset, plugin_message_data);

	packet->ptype = SBPTYPE_PLUGIN_MESSAGE;
	packet->data.plugin_message.channel = channel;
	packet->data.plugin_message.data = plugin_message_data;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_24(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	uuid target_player;

	DO_READ(read_uuid, target_player);
	packet->ptype = SBPTYPE_SPECTATE;
	packet->data.spectate.target_player[0] = target_player[0];
	packet->data.spectate.target_player[1] = target_player[1];
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

static read_result read_play_25(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	string hash;
	int32_t resource_pack_status_result;

	DO_READ(read_string, hash);
	DO_READ(read_varint, resource_pack_status_result);
	packet->ptype = SBPTYPE_RESOURCE_PACK_STATUS;
	packet->data.resource_pack_status.hash = hash;
	packet->data.resource_pack_status.result = resource_pack_status_result;
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
