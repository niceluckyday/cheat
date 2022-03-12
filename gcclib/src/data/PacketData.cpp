#include "pch.h"
#include <gcclib/data/PacketData.h>

void PacketData::Write(PipeTransfer* transfer)
{
	transfer->Write<bool>(waitForModifyData);
	transfer->Write<bool>(valid);
	transfer->Write<PacketType>(type);
	transfer->Write<int16_t>(messageId);
	transfer->WriteVector(headData);
	transfer->WriteVector(messageData);
}

void PacketData::Read(PipeTransfer* transfer)
{
	waitForModifyData = transfer->Read<bool>();
	valid = transfer->Read<bool>();
	type = transfer->Read<PacketType>();
	messageId = transfer->Read<int16_t>();
	headData = transfer->ReadVector();
	messageData = transfer->ReadVector();
}
