#include "pch-il2cpp.h"
#include "PacketSniffer.h"

#include <imgui.h>
#include <common/util.h>
#include <common/HookManager.h>
#include <gui/gui-util.h>
#include <helpers.h>
#include <gcclib/Globals.h>

namespace cheat::feature 
{

	static int32_t KcpNative_kcp_client_send_packet_Hook(void* __this, void* kcp_client, app::KcpPacket_1* packet, MethodInfo* method);
	static bool KcpClient_TryDequeueEvent_Hook(void* __this, app::ClientKcpEvent* evt, MethodInfo* method);

	PacketSniffer::PacketSniffer() : Feature(),
		NF(m_CapturingEnabled, "Capturing", "PacketSniffer", false),
		NF(m_ManipulationEnabled, "Manipulation", "PacketSniffer", false),
		m_NextTimeToConnect(0),
		m_Pipe({ Globals::packetPipeName })
	{
		HookManager::install(app::KcpNative_kcp_client_send_packet, KcpNative_kcp_client_send_packet_Hook);
		HookManager::install(app::KcpClient_TryDequeueEvent, KcpClient_TryDequeueEvent_Hook);

		if (m_CapturingEnabled && !TryConnectToPipe())
			LOG_WARNING("Failed connect to pipe.");
	}

	const FeatureGUIInfo& PacketSniffer::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "Packet sniffer", "Settings", true };
		return info;
	}

	void PacketSniffer::DrawMain()
	{
		ImGui::Text("Dev: for working needs server for named pipe 'genshin_packet_pipe'.\nCheck 'packet-handler' project like example.");
		ConfigWidget(m_CapturingEnabled, "Enabling capturing packets info and send it to pipe, if exists.");
		ConfigWidget(m_ManipulationEnabled, "Enabling manipulation packet feature, that allows to replace, block incoming/outcoming packets." \
			"\nThis feature often needs, to read-write pipe operation, so can decrease network bandwidth.");
	}

	PacketSniffer& PacketSniffer::GetInstance()
	{
		static PacketSniffer instance;
		return instance;
	}

	bool PacketSniffer::OnPacketIO(app::KcpPacket_1* packet, PacketType type)
	{
		if (!m_CapturingEnabled)
			return true;

		if (!m_Pipe.IsPipeOpened() && !TryConnectToPipe())
			return true;

		PacketData data = ParseRawPacketData((char*)packet->data, packet->dataLen);
		if (!data.valid)
			return true;

		data.waitForModifyData = m_ManipulationEnabled;
		data.type = type;
		SendData(data);

		if (m_ManipulationEnabled)
		{
			auto modifyData = ReceiveData();
			if (modifyData.type == PacketModifyType::Blocked)
				return false;

			if (modifyData.type == PacketModifyType::Modified)
			{
				auto dataSize = modifyData.modifiedData.size();
				packet->data = new byte[dataSize]();
				memcpy_s(packet->data, dataSize, modifyData.modifiedData.data(), dataSize);
				packet->dataLen = dataSize;
			}
		}
		return true;
	}

	bool PacketSniffer::TryConnectToPipe()
	{
		std::time_t currTime = std::time(0);
		if (m_NextTimeToConnect > currTime)
			return false;
		
		bool result = m_Pipe.Connect();
		if (result)
			LOG_INFO("Connected to pipe successfully.");
		else
			m_NextTimeToConnect = currTime + 5; // delay in 5 sec
		return result;
	}

	char* PacketSniffer::EncryptXor(void* content, uint32_t length)
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

	bool PacketSniffer::isLittleEndian()
	{
		unsigned int i = 1;
		char* c = (char*)&i;
		return (*c);
	}

	PacketData PacketSniffer::ParseRawPacketData(char* encryptedData, uint32_t length)
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

	void PacketSniffer::SendData(PacketData& data)
	{
		if (m_Pipe.IsPipeOpened())
		{
			//LOG_DEBUG("%s data with mid %d.", magic_enum::enum_name(data.type).data(), data.messageId);
			m_Pipe.WriteObject(data);
		}
	}

	PacketModifyData PacketSniffer::ReceiveData()
	{
		PacketModifyData md{};
		if (m_Pipe.IsPipeOpened())
		{
			m_Pipe.ReadObject(md);
		}
		return md;
	}

	static bool KcpClient_TryDequeueEvent_Hook(void* __this, app::ClientKcpEvent* evt, MethodInfo* method)
	{
		auto result = callOrigin(KcpClient_TryDequeueEvent_Hook, __this, evt, method);

		if (!result || evt->_evt.type != app::KcpEventType__Enum::EventRecvMsg ||
			evt->_evt.packet == nullptr || evt->_evt.packet->data == nullptr)
			return result;

		auto& sniffer = PacketSniffer::GetInstance();
		return sniffer.OnPacketIO(evt->_evt.packet, PacketType::Receive);
	}

	static int32_t KcpNative_kcp_client_send_packet_Hook(void* __this, void* kcp_client, app::KcpPacket_1* packet, MethodInfo* method)
	{
		auto& sniffer = PacketSniffer::GetInstance();
		if (!sniffer.OnPacketIO(packet, PacketType::Send))
			return 0;

		return callOrigin(KcpNative_kcp_client_send_packet_Hook, __this, kcp_client, packet, method);
	}
}

