#include "mem.h"
#include "packet/protocol_47.h"

/* encryption_response */
read_result protocol_47_read_login_1(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	(void)result;
	(void)size;
	(void)data;
	(void)offset;

	/* Declare data types and read them. */

	packet->ptype = PROTOCOL_47_SB_ENCRYPTION_RESPONSE;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

/* use_entity */
read_result protocol_47_read_play_2(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	(void)result;
	(void)size;
	(void)data;
	(void)offset;
	/* Declare data types and read them. */

	packet->ptype = PROTOCOL_47_SB_USE_ENTITY;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

/* player_block_placement */
read_result protocol_47_read_play_8(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	(void)result;
	(void)size;
	(void)data;
	(void)offset;

	/* Declare data types and read them. */

	packet->ptype = PROTOCOL_47_SB_PLAYER_BLOCK_PLACEMENT;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

/* click_window */
read_result protocol_47_read_play_14(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	(void)result;
	(void)size;
	(void)data;
	(void)offset;

	/* Declare data types and read them. */

	packet->ptype = PROTOCOL_47_SB_CLICK_WINDOW;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

/* creative_inventory_action */
read_result protocol_47_read_play_16(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	(void)result;
	(void)size;
	(void)data;
	(void)offset;

	/* Declare data types and read them. */

	packet->ptype = PROTOCOL_47_SB_CREATIVE_INVENTORY_ACTION;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

/* tab_complete */
read_result protocol_47_read_play_20(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	(void)result;
	(void)size;
	(void)data;
	(void)offset;

	/* Declare data types and read them. */

	packet->ptype = PROTOCOL_47_SB_TAB_COMPLETE;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}

/* plugin_message */
read_result protocol_47_read_play_23(bytearray *data, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	(void)result;
	(void)size;
	(void)data;
	(void)offset;

	/* Declare data types and read them. */

	packet->ptype = PROTOCOL_47_SB_PLUGIN_MESSAGE;

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
