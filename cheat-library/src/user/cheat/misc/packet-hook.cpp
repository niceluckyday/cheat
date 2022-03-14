#include "pch-il2cpp.h"
#include <cheat/cheat.h>

#include <iostream>
#include <filesystem>
#include <ctime>

#include <magic_enum.hpp>

#include <helpers.h>
#include <common/HookManager.h>
#include <common/Config.h>
#include <gcclib/PipeTransfer.h>
#include <gcclib/Globals.h>
#include <gcclib/data/PacketData.h>
#include <gcclib/data/PacketModifyData.h>

static PipeTransfer pipe = { Globals::packetPipeName };
static std::time_t nextTimeToConnect = 0;

bool TryConnectToPipe() 
{
	std::time_t currTime = std::time(0);
	if (nextTimeToConnect > currTime)
		return false;

	bool result = pipe.Connect();
	if (result)
		LOG_INFO("Connected to pipe successfully.");
	else
		nextTimeToConnect = currTime + 5; // delay in 5 sec
	return result;
}

static char* EncryptXor(void* content, uint32_t length)
{
	app::Byte__Array* byteArray = (app::Byte__Array*)new char[length + 0x20];
	byteArray->max_length = length;
	memcpy_s(byteArray->vector, length, content, length);

	app::Packet_XorEncrypt(nullptr, &byteArray, length, nullptr);
	
	auto result = new char[length];
	memcpy_s(result, length, byteArray->vector, length);
	delete[] byteArray;

	return (char*)result;
}

static bool isLittleEndian() 
{
	unsigned int i = 1;
	char* c = (char*)&i;
	return (*c);
}

template<class T>
static T read(char* data, int offset, bool littleEndian = false) 
{
	T result = {};
	if (isLittleEndian() != littleEndian) 
	{
		for (int i = 0; i < sizeof(T); i++)
			((char*)&result)[i] = data[offset + sizeof(T) - i - 1];
		return result;
	}
	memcpy_s(&result, sizeof(result), data + offset, sizeof(result));
	return result;
}

static PacketData ParseRawPacketData(char* encryptedData, uint32_t length)
{
	// Decrypting data
	auto data = EncryptXor(encryptedData, length);

	uint16_t magicHead = read<uint16_t>(data, 0);
	
	if (magicHead != 0x4567)
	{
		LOG_ERROR("Head magic value for packet is not valid.");
		return {};
	}

	uint16_t magicEnd = read<uint16_t>(data, length - 2);
	if (magicEnd != 0x89AB)
	{
		LOG_ERROR("End magic value for packet is not valid.");
		return {};
	}

	uint16_t messageId = read<uint16_t>(data, 2);
	uint16_t headSize = read<uint16_t>(data, 4);
	uint32_t contenSize = read<uint32_t>(data, 6);
	
	if (length < headSize + contenSize + 12) 
	{
		LOG_ERROR("Packet size is not valid.");
		return {};
	}

	PacketData packetData = {};
	packetData.valid = true;
	packetData.messageId = messageId;

	packetData.headData = std::vector<byte>((size_t)headSize, 0);
	memcpy_s(packetData.headData.data(), headSize, data + 10, headSize);

	packetData.messageData = std::vector<byte>((size_t)contenSize, 0);
	memcpy_s(packetData.messageData.data(), contenSize, data + 10 + headSize, contenSize);

	delete[] data;

	return packetData;
}

void SendData(PacketData& data)
{
	if (pipe.IsPipeOpened())
	{
		//LOG_DEBUG("%s data with mid %d.", magic_enum::enum_name(data.type).data(), data.messageId);
		pipe.WriteObject(data);
	}
}

PacketModifyData ReceiveData() 
{
	PacketModifyData md {};
	if (pipe.IsPipeOpened()) 
	{
		pipe.ReadObject(md);
	}
	return md;
}

static int32_t KcpNative_kcp_client_send_packet_Hook(void* __this, void* kcp_client, app::KcpPacket_1* packet, MethodInfo* method) 
{
	if (!Config::cfgPacketCapturing.GetValue())
		return callOrigin(KcpNative_kcp_client_send_packet_Hook, __this, kcp_client, packet, method);

	if (!pipe.IsPipeOpened() && !TryConnectToPipe())
		return callOrigin(KcpNative_kcp_client_send_packet_Hook, __this, kcp_client, packet, method);

	PacketData data = ParseRawPacketData((char*)packet->data, packet->dataLen);
	if (!data.valid) 
		return callOrigin(KcpNative_kcp_client_send_packet_Hook, __this, kcp_client, packet, method);

	data.waitForModifyData = Config::cfgPacketManipulation.GetValue();
	data.type = PacketType::Send;
	SendData(data);

	if (Config::cfgPacketManipulation.GetValue()) 
	{
		auto modifyData = ReceiveData();
		if (modifyData.type == PacketModifyType::Blocked)
			return 0;

		if (modifyData.type == PacketModifyType::Modified)
		{
			auto dataSize = modifyData.modifiedData.size();
			packet->data = new byte[dataSize]();
			memcpy_s(packet->data, dataSize, modifyData.modifiedData.data(), dataSize);
			packet->dataLen = dataSize;
		}
	}
	return callOrigin(KcpNative_kcp_client_send_packet_Hook, __this, kcp_client, packet, method);
}

bool KcpClient_TryDequeueEvent_Hook(void* __this, app::ClientKcpEvent* evt, MethodInfo* method) 
{
	auto result = callOrigin(KcpClient_TryDequeueEvent_Hook, __this, evt, method);

	if (!Config::cfgPacketCapturing.GetValue())
		return result;
	
	if (!pipe.IsPipeOpened() && !TryConnectToPipe())
		return result;
	
	if (evt->_evt.type != app::KcpEventType__Enum::EventRecvMsg ||
		evt->_evt.packet == nullptr || evt->_evt.packet->data == nullptr)
		return result;


	PacketData data = ParseRawPacketData((char*)evt->_evt.packet->data, evt->_evt.packet->dataLen);
	if (!data.valid)
		return result;

	data.waitForModifyData = Config::cfgPacketManipulation.GetValue();
	data.type = PacketType::Receive;
	SendData(data);

	if (Config::cfgPacketManipulation.GetValue())
	{
		auto modifyData = ReceiveData();
		if (modifyData.type == PacketModifyType::Blocked)
			return false;

		if (modifyData.type == PacketModifyType::Modified)
		{
			auto packet = evt->_evt.packet;
			auto dataSize = modifyData.modifiedData.size();
			packet->data = new byte[dataSize]();
			memcpy_s(packet->data, dataSize, modifyData.modifiedData.data(), dataSize);
			packet->dataLen = dataSize;
		}
	}

	return result;
}

void InitPacketHooks() 
{
	HookManager::install(app::KcpNative_kcp_client_send_packet, KcpNative_kcp_client_send_packet_Hook);
	HookManager::install(app::KcpClient_TryDequeueEvent, KcpClient_TryDequeueEvent_Hook);

	if (Config::cfgPacketCapturing.GetValue() && !TryConnectToPipe())
		LOG_WARNING("Failed connect to pipe.");
}