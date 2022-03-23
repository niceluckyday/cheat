#pragma once
#include <cheat-base/PipeTransfer.h>

enum class PacketType 
{
	Receive,
	Send
};

class PacketData : public PipeSerializedObject
{
public:
	bool waitForModifyData;

	bool valid;
	PacketType type;
	int16_t messageId;
	std::vector<byte> headData;
	std::vector<byte> messageData;

	// Inherited via PipeSerializedObject
	virtual void Write(PipeTransfer* transfer) override;
	virtual void Read(PipeTransfer* transfer) override;
};