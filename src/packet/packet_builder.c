#include "packet/packet_builder.h"
#include "mem.h"
#include "packet/write_fn.h"
#include "util/bytearray.h"
#include <uuid/uuid.h>

void packet_build(packet_builder *builder, uint32_t id)
{
	bytearray_init(&builder->buffer);
	builder->id = id;
}

void packet_append_varint(packet_builder *builder, int32_t varint)
{
	write_varint(&builder->buffer, builder->buffer.size, varint, NULL);
}

void packet_append_string(packet_builder *builder, const char *str)
{
	write_string(&builder->buffer, builder->buffer.size, str, NULL);
}
void packet_append_bool(packet_builder *builder, bool b)
{
	write_bool(&builder->buffer, builder->buffer.size, b, NULL);
}
void packet_append_i8(packet_builder *builder, int8_t i8)
{
	write_i8(&builder->buffer, builder->buffer.size, i8, NULL);
}
void packet_append_u8(packet_builder *builder, uint8_t u8)
{
	write_u8(&builder->buffer, builder->buffer.size, u8, NULL);
}
void packet_append_i16(packet_builder *builder, int16_t i16)
{
	write_i16(&builder->buffer, builder->buffer.size, i16, NULL);
}
void packet_append_u16(packet_builder *builder, uint16_t u16)
{
	write_u16(&builder->buffer, builder->buffer.size, u16, NULL);
}
void packet_append_i32(packet_builder *builder, int32_t i32)
{
	write_i32(&builder->buffer, builder->buffer.size, i32, NULL);
}
void packet_append_i64(packet_builder *builder, int64_t i64)
{
	write_i64(&builder->buffer, builder->buffer.size, i64, NULL);
}
void packet_append_u64(packet_builder *builder, uint32_t u64)
{
	write_u64(&builder->buffer, builder->buffer.size, u64, NULL);
}
void packet_append_float(packet_builder *builder, float f)
{
	write_float(&builder->buffer, builder->buffer.size, f, NULL);
}
void packet_append_double(packet_builder *builder, double d)
{
	write_double(&builder->buffer, builder->buffer.size, d, NULL);
}
void packet_append_position(packet_builder *builder, ipos *pos)
{
	write_position(&builder->buffer, builder->buffer.size, pos, NULL);
}
void packet_append_bytearray(packet_builder *builder, bytearray *to_write)
{
	write_bytearray(&builder->buffer, builder->buffer.size, to_write, NULL);
}
void packet_append_uuid(packet_builder *builder, uuid_t uuid)
{
	write_uuid(&builder->buffer, builder->buffer.size, uuid, NULL);
}

void packet_package(packet_builder *builder)
{
	/* packet id. */
	write_varint(&builder->buffer, 0, builder->id, NULL);

	/* length. */
	write_varint(&builder->buffer, 0, builder->buffer.size, NULL);
}

void packet_free_builder(packet_builder *builder)
{
	blue_free(builder->buffer.data);
}
