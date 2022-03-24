#pragma once
#include <cheat-base/PipeTransfer.h>

enum class PacketModifyType 
{
	Modified,
	Blocked,
	Unchanged
};

class PacketModifyData : public PipeSerializedObject
{
public:
	PacketModifyData() : type(PacketModifyType::Unchanged), modifiedData() {}
	~PacketModifyData() {}
	
	PacketModifyType type;
	std::string modifiedData;

	// Inherited via PipeSerializedObject
	virtual void Write(PipeTransfer* transfer) override;
	virtual void Read(PipeTransfer* transfer) override;
};

