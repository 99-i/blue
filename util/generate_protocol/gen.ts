import {log} from "console";
import protocol47data from "./protocols/47.json";
import {mkdir, mkdirSync, writeFileSync} from "fs";
import {getHeapSnapshot} from "v8";
import {hash} from "crypto";
import {hasSubscribers} from "diagnostics_channel";

type ProtocolState = "Handshaking" | "Status" | "Login" | "Play";
type Direction = "Serverbound" | "Clientbound";

const typeTranslations: Record<any, any> = {
	VarInt: "int32_t",
	Long: "int64_t", String: "string", Boolean: "bool",
	"Unsigned Short": "uint16_t",
	Double: "double",
	Float: "float",
	Byte: "int8_t",
	"Unsigned Byte": "uint8_t",
	Short: "int16_t",
	Chat: "chat_obj",
	UUID: "uuid_t",
	Position: "ipos",
	"Byte Enum": "int8_t",
	"VarInt Enum": "int32_t",
};

const readTranslations: Record<any, any> = {
	VarInt: "read_varint",
	Long: "read_i64",
	String: "read_string",
	Boolean: "read_bool",
	"Unsigned Short": "read_u16",
	Double: "read_double",
	Float: "read_float",
	Byte: "read_i8",
	"Unsigned Byte": "read_u8",
	Short: "read_i16",
	Chat: "read_chat",
	UUID: "read_uuid",
	Position: "read_position",
	"Byte Enum": "read_i8",
	"VarInt Enum": "read_i32",
}

interface ProtocolPacketField {
	name: string;
	type: string;
	notes: string;
}

interface ProtocolPacket {
	name: string;
	id: number;
	state: ProtocolState;
	direction: Direction;
	fields: ProtocolPacketField[];
}

interface Protocol {
	version: number;
	serverboundPackets: ProtocolPacket[];
	clientboundPackets: ProtocolPacket[];
}

const protocol47 = protocol47data as Protocol;

const nameifyLowercase = (name: string) => {
	return name.toLowerCase().split(" ").join("_").split("-").join("_");
};

const nameifyUppercase = (name: string) => {
	return name.toUpperCase().split(" ").join("_").split("-").join("_");
};

//sort serverbound.
function sortPackets(protocol: Protocol) {
	protocol.serverboundPackets.sort((a, b) => {
		const precedence = {
			Handshaking: 0,
			Status: 1,
			Login: 2,
			Play: 3,
		};

		return precedence[a.state] - precedence[b.state];
	});
}

function canPacketBeGenerated(packet: ProtocolPacket):
	| {
		generable: true;
	}
	| {
		generable: false;
		reason: string[];
	} {
	const permissibleFieldTypes = [
		"VarInt",
		"Long",
		"String",
		"Boolean",
		"Unsigned Short",
		"Double",
		"Float",
		"Byte",
		"Unsigned Byte",
		"Short",
		"Chat",
		"UUID",
		"Position",
		"Byte Enum",
		"VarInt Enum",
	];

	const reasons: string[] = [];

	packet.fields.forEach((field) => {
		if (!permissibleFieldTypes.includes(field.type)) {
			reasons.push(`Unknown type "${field.type}" of field "${field.name}"`);
		}
	});

	if (reasons.length === 0) {
		return {
			generable: true,
		};
	} else {
		return {
			generable: false,
			reason: reasons,
		};
	}
}

function generatePacketEnums(protocol: Protocol): string {
	let out = "typedef enum {\n";
	protocol.serverboundPackets.forEach((packet) => {
		out += `\tPROTOCOL_${protocol.version}_SB_${nameifyUppercase(
			packet.name
		)},\n`;
	});
	out = out.substring(0, out.length - 2);
	out += `\n} protocol_${protocol.version}_sb_packet_type;`;
	return out;
}

function generateStructDeclarations(protocol: Protocol): string {
	let out = `typedef struct
{
	protocol_${protocol.version}_sb_packet_type ptype;
	union
	{`;
	protocol.serverboundPackets.forEach((packet) => {
		let generable = canPacketBeGenerated(packet);
		if (generable.generable) {
			out += `\n\t\tstruct\n\t\t{\n${(() => {
				let out2 = "";
				packet.fields.forEach((field) => {
					if (field.notes.length > 0) out2 += `\t\t\t/* ${field.notes} */\n`;
					out2 += `\t\t\t${typeTranslations[field.type]} ${nameifyLowercase(
						field.name
					)};\n`;
				});
				return out2;
			})()}\t\t} ${nameifyLowercase(packet.name)};`;
		} else {
			out += `\n\t\t/*struct {TODO} ${nameifyLowercase(packet.name)}; */`;
		}
	});
	out += `\n\t} data;`;

	// out = out.substring(0, out.length - 1);
	out += `\n} protocol_${protocol.version}_serverbound_packet;`;

	return out;
}

function getDeclaration(protocol: Protocol, packet: ProtocolPacket): {
	dec: string,
	isExt: boolean
} {
	let dec = ""
	dec += `read_result protocol_${protocol.version}_read_${nameifyLowercase(
		packet.state
	)}_${packet.id}(bytearray *data, size_t offset, protocol_${protocol.version
		}_serverbound_packet *packet, uint32_t *bytes_read)`;

	return {
		dec,
		isExt: !canPacketBeGenerated(packet).generable
	}
}

function getAllDeclarations(protocol: Protocol): string {
	let dec = ""
	protocol.serverboundPackets.forEach((packet) => {
		let packetDec = getDeclaration(protocol, packet)
		if (packetDec.isExt) {
			dec += packetDec.dec + `; /* EXT */\n`
		} else {
			dec += packetDec.dec + `;\n`
		}
	});
	return dec
}

function generateProtocolH(protocol: Protocol): string {
	let header = `#pragma once
#include "game/client_event.h"
#include "net/protocol.h"
#include "types.h"
#include "util/bytearray.h"
#include <stdbool.h>

typedef struct
{
} protocol_${protocol.version}_state;

protocol_${protocol.version}_state *protocol_${protocol.version}_create_state(void);
read_result protocol_${protocol.version}_pop_client_event(void *state, client_state c_state, bytearray *read_buffer, client_event *event);

${generatePacketEnums(protocol)}

${generateStructDeclarations(protocol)}

read_result protocol_${protocol.version}_read_packet(bytearray *data, size_t offset, protocol_${protocol.version}_serverbound_packet *packet, uint32_t *bytes_read);

${getAllDeclarations(protocol)}
`;


	return header;
}

function generateProtocolC(protocol: Protocol): string {
	let dec = `#include "packet/protocol_${protocol.version}/protocol.h"
#include "mem.h"
#include "packet/read_fn.h"

protocol_${protocol.version}_state *protocol_${protocol.version}_create_state(void)
{
	protocol_${protocol.version}_state *state = blue_malloc(sizeof(*state));
	return state;
}

static read_result protocol_${protocol.version}_pop_packet(bytearray *read_buffer, client_state c_state, void *state, protocol_${protocol.version}_serverbound_packet *packet);

read_result protocol_${protocol.version}_pop_client_event(void *state, client_state c_state, bytearray *read_buffer, client_event *event)
{
	read_result result;
	protocol_${protocol.version}_serverbound_packet packet;
	uint32_t bytes_read;

	/* Attempt to pop packet. */
	result = protocol_${protocol.version}_pop_packet(read_buffer, c_state, state, &packet);

	if(result) return result;

	/* Convert to client event.*/

}
static read_result protocol_${protocol.version}_pop_packet(bytearray *read_buffer, client_state c_state, void *state, protocol_${protocol.version}_serverbound_packet *packet)
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

	printf("Read packet with length %d\\n", packet_length);

	/* construct temporary buffer with size of packet length. */

	temporary_packet_buffer.capacity = packet_length;
	temporary_packet_buffer.size = packet_length;
	temporary_packet_buffer.data = read_buffer->data;

	result = protocol_${protocol.version}_read_packet(&temporary_packet_buffer, c_state, size, packet, &size_buffer);
	if(result) return result;
	if((int32_t) size_buffer != packet_length) {
		return READ_RESULT_MALFORMED;
	}

	bytearray_slice_from_beginning(read_buffer, size + size_buffer);

	return READ_RESULT_SUCCESS;
}

read_result protocol_${protocol.version}_read_packet(bytearray *data, client_state c_state, size_t offset, protocol_47_serverbound_packet *packet, uint32_t *bytes_read)
{
	int32_t id;
	uint32_t size;
	read_result result;
	${(() => {
			let out = ""
			const states = ['status', 'login', 'play']
			states.forEach((v) => {
				const statesPackets = protocol.serverboundPackets.filter((packet) => {
					return packet.state.toLowerCase() === v
				})
				out += `static const protocol_${protocol.version}_read_packet_fn read_${v}[${statesPackets.length}] = {${(() => {
					let out2 = ""
					statesPackets.forEach((packet) => {
						out2 += `protocol_${protocol.version}_read_${v}_${packet.id}, `
					})
					out2 = out2.substring(0, out2.length - 2)
					return out2
				})()}};\n`
			})
			return out
		})()}

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

#define DO_READ(read_fn, variable_name)                                        \
	result = read_fn(data, offset + bytes_read_buffer, &variable_name, &size); \
	if (result)                                                                \
		return result;                                                         \
	bytes_read_buffer += size;

#define DO_READ_BUFFER(buffer_size, variable_name)                            \
	result = read_bytearray(data, offset + bytes_read_buffer, &variable_name, \
							buffer_size, &size);                              \
	if (result)                                                               \
		return result;                                                        \
	bytes_read_buffer += size;

	${(() => {

			//static read_result read_status_1(bytearray *data, size_t offset, serverbound_47_packet *packet, uint32_t *bytes_read)
			//{
			//	uint32_t bytes_read_buffer = 0;
			//	uint32_t size;
			//	read_result result;
			//
			//	int64_t payload;
			//
			//	DO_READ(read_i64, payload);
			//	packet->ptype = SBPTYPE_PING;
			//	packet->data.ping.payload = payload;
			//	if (bytes_read)
			//	{
			//		*bytes_read = bytes_read_buffer;
			//	}
			//	return READ_RESULT_SUCCESS;
			//}
			let out = ""
			protocol.serverboundPackets.forEach((packet) => {
				const dec = getDeclaration(protocol, packet)
				if (!dec.isExt) {
					out += `${dec.dec}
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;
	
${(() => {
							let out2 = ""
							packet.fields.forEach((field) => {
								out2 += `\t${typeTranslations[field.type]} ${nameifyLowercase(field.name)};\n`
							})
							return out2
						})()}
${(() => {
							let out2 = ""
							packet.fields.forEach((field) => {
								out2 += `\tDO_READ(${readTranslations[field.type]}, ${nameifyLowercase(field.name)});\n`
							})
							return out2
						})()}
	packet->ptype = PROTOCOL_${protocol.version}_SB_${nameifyUppercase(
							packet.name
						)};
${(() => {
							let out2 = ""
							packet.fields.forEach((field) => {
								out2 += `\tpacket->data.${nameifyLowercase(packet.name)}.${nameifyLowercase(field.name)} = ${nameifyLowercase(field.name)};\n`
							})
							return out2
						})()}
	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}
`
				}
			})
			return out
		})()}
`
	return dec
}

function generateFilesForProtocol(protocol: Protocol): void {
	const protocolH = generateProtocolH(protocol);
	const protocolC = generateProtocolC(protocol);
	const extC = genereateExtC(protocol)
	//log(protocolH);
	log(extC);

	writeFileSync(`out/protocol_${protocol.version}/protocol.h`, protocolH);
	writeFileSync(`out/protocol_${protocol.version}/protocol.c`, protocolC);
	writeFileSync(`out/protocol_${protocol.version}/ext.c`, extC);
}

sortPackets(protocol47);

generateFilesForProtocol(protocol47);

function genereateExtC(protocol: Protocol): string {
	let out = `#include "packet/protocol_${protocol.version}/protocol.h"
#include "mem.h"

`
	protocol.serverboundPackets.forEach((packet) => {
		const dec = getDeclaration(protocol, packet)
		if (dec.isExt) {
			out += `
/* ${nameifyLowercase(packet.name)} */
${dec.dec}
{
	uint32_t bytes_read_buffer = 0;
	uint32_t size;
	read_result result;

	/* Declare data types and read them. */

	packet->ptype = PROTOCOL_${protocol.version}_SB_${nameifyUppercase(packet.name)};

	if (bytes_read)
	{
		*bytes_read = bytes_read_buffer;
	}
	return READ_RESULT_SUCCESS;
}\n`
		}
	})

	return out
}
