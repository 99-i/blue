import { log } from "console";

import protocol47data from "./protocols/47.json";
import { mkdir, mkdirSync, writeFileSync } from "fs";

type ProtocolState = "Handshaking" | "Status" | "Login" | "Play";
type Direction = "Serverbound" | "Clientbound";

const typeTranslations: Record<any, any> = {
  VarInt: "int32_t",
  Long: "int64_t",
  String: "string",
  Boolean: "bool",
  "Unsigned Short": "uint16_t",
  Double: "double",
  Float: "float",
  Byte: "int8_t",
  "Unsigned Byte": "uint8_t",
  Short: "int16_t",
  Chat: "chat_obj",
  UUID: "uuid",
  Position: "ipos",
  "Byte Enum": "int8_t",
  "VarInt Enum": "int32_t",
};

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

function generateProtocolH(protocol: Protocol): string {
  let header = `#pragma once
#include "game/event.h"
#include "net/protocol.h"
#include "types.h"
#include "util/bytearray.h"
#include <stdbool.h>

typedef struct                                  
{                                               
	bytearray read_buffer;                      
	client_state state;                         
} protocol_${protocol.version}_state;

protocol_${protocol.version}_state *create_state(void);
void *append_data(void *state, const uint8_t *data, size_t size);
read_result *pop_game_event(void *state, game_event *event);

${generatePacketEnums(protocol)}

${generateStructDeclarations(protocol)}

`;

  protocol.serverboundPackets.forEach((packet) => {
    header += `read_result protocol_${protocol.version}_read_${nameifyLowercase(
      packet.state
    )}_${packet.id}(bytearray *data, size_t offset, protocol_${
      protocol.version
    }_serverbound_packet *packet, uint32_t *bytes_read);${(() => {
      let generability = canPacketBeGenerated(packet);
      if (!generability.generable) {
        return ` /* EXT */`;
      }
      return "";
    })()}
`;
  });

  header += "\n";

  return header;
}

function generateFilesForProtocol(protocol: Protocol): void {
  const protocolH = generateProtocolH(protocol);

  log(protocolH);

  writeFileSync("out/protocol_47/protocol.h", protocolH);
}

sortPackets(protocol47);

generateFilesForProtocol(protocol47);
