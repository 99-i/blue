#pragma once
#include "types.h"
#include "util/bytearray.h"
#include <stdbool.h>
#include <uuid/uuid.h>

typedef struct
{
	bytearray buffer;
	uint32_t id;
} packet_builder;

void packet_build(packet_builder *builder, uint32_t id);

void packet_append_varint(packet_builder *builder, int32_t varint);
void packet_append_string(packet_builder *builder, const char *str);
void packet_append_bool(packet_builder *builder, bool b);
void packet_append_i8(packet_builder *builder, int8_t i8);
void packet_append_u8(packet_builder *builder, uint8_t u8);
void packet_append_i16(packet_builder *builder, int16_t i16);
void packet_append_u16(packet_builder *builder, uint16_t u16);
void packet_append_i32(packet_builder *builder, int32_t i32);
void packet_append_i64(packet_builder *builder, int64_t i64);
void packet_append_u64(packet_builder *builder, uint32_t u64);
void packet_append_float(packet_builder *builder, float f);
void packet_append_double(packet_builder *builder, double d);
void packet_append_position(packet_builder *builder, ipos *pos);
void packet_append_bytearray(packet_builder *builder, bytearray *to_write);
void packet_append_uuid(packet_builder *builder, uuid_t uuid);

/* append id and length varint. */
void packet_package(packet_builder *builder);

/* frees builder->buffer */
void packet_free_builder(packet_builder *builder);
