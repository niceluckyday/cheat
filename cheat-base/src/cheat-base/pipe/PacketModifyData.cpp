#include <pch.h>
#include "PacketModifyData.h"

void PacketModifyData::Write(PipeTransfer* transfer)
{
	transfer->Write<PacketModifyType>(type);
	if (type == PacketModifyType::Modified)
		transfer->WriteString(modifiedData);
}

void PacketModifyData::Read(PipeTransfer* transfer)
{
	type = transfer->Read<PacketModifyType>();
	if (type == PacketModifyType::Modified) 
		modifiedData = transfer->ReadString();
}
