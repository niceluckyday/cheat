#include <pch.h>
#include "PacketData.h"

void PacketData::Write(PipeTransfer* transfer)
{
	transfer->Write<bool>(waitForModifyData);
	transfer->Write<bool>(valid);
	transfer->Write<PacketType>(type);
	transfer->Write<int16_t>(messageId);
	transfer->WriteString(name);
	transfer->WriteString(headJson);
	transfer->WriteVector(headData);
	transfer->WriteString(messageJson);
	transfer->WriteVector(messageData);
	
}

void PacketData::Read(PipeTransfer* transfer)
{
	waitForModifyData = transfer->Read<bool>();
	valid = transfer->Read<bool>();
	type = transfer->Read<PacketType>();
	messageId = transfer->Read<int16_t>();
	name = transfer->ReadString();

	headJson = transfer->ReadString();
	headData = transfer->ReadVector();
	
	messageJson = transfer->ReadString();
	messageData = transfer->ReadVector();
}
