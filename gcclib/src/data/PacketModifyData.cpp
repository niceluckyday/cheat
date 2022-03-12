#include "pch.h"
#include <gcclib/data/PacketModifyData.h>

void PacketModifyData::Write(PipeTransfer* transfer)
{
	transfer->Write<PacketModifyType>(type);
	if (type == PacketModifyType::Modified)
		transfer->WriteVector(modifiedData);
}

void PacketModifyData::Read(PipeTransfer* transfer)
{
	type = transfer->Read<PacketModifyType>();
	if (type == PacketModifyType::Modified) 
	{
		modifiedData = transfer->ReadVector();
	}
		
}
