#include "pch-il2cpp.h"
#include "PacketSniffer.h"

#include <fstream>

#include <helpers.h>

namespace cheat::feature 
{

	static int32_t KcpNative_kcp_client_send_packet_Hook(void* __this, void* kcp_client, app::KcpPacket_1* packet, MethodInfo* method);
	static bool KcpClient_TryDequeueEvent_Hook(void* __this, app::ClientKcpEvent* evt, MethodInfo* method);

	PacketSniffer::PacketSniffer() : Feature(),
		NF(m_CapturingEnabled, "Capturing", "PacketSniffer", false),
		NF(m_ManipulationEnabled, "Manipulation", "PacketSniffer", false),
		NF(m_PipeEnabled, "Pipe", "PacketSniffer", false),
		NF(m_ProtoDirPath, "Proto dir path", "PacketSniffer", ""),
		NF(m_ProtoIDFilePath, "Proto id file path", "PacketSniffer", ""),
		m_ProtoManager(m_ProtoIDFilePath, m_ProtoDirPath),
		m_NextTimeToConnect(0),
		m_Pipe({ "genshin_packet_pipe" })
	{
		HookManager::install(app::KcpNative_kcp_client_send_packet, KcpNative_kcp_client_send_packet_Hook);
		HookManager::install(app::KcpClient_TryDequeueEvent, KcpClient_TryDequeueEvent_Hook);

		if (m_CapturingEnabled && m_PipeEnabled && !TryConnectToPipe())
			LOG_WARNING("Failed connect to pipe.");
	}

	const FeatureGUIInfo& PacketSniffer::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "Packet sniffer", "Settings", true };
		return info;
	}

	bool PacketSniffer::OnCapturingChanged()
	{
		if (!m_CapturingEnabled)
			return true;

		if (!m_ProtoDirPath.value().empty() && !m_ProtoIDFilePath.value().empty())
		{
			m_ProtoManager.LoadIDFile(m_ProtoIDFilePath);
			m_ProtoManager.LoadProtoDir(m_ProtoDirPath);
			return true;
		}

		return false;
	}

	void PacketSniffer::DrawMain()
	{
		//ImGui::Text("Dev: for working needs server for named pipe 'genshin_packet_pipe'.\nCheck 'packet-handler' project like example.");
		if (ConfigWidget(m_CapturingEnabled, "Enabling capturing packets info and send it to pipe, if exists."))
		{ 
			bool result = OnCapturingChanged();
			if (!result)
			{
				*m_CapturingEnabled = false;
				m_CapturingEnabled.Check();
				ImGui::OpenPopup("Error");
			}
		}

		if (ImGui::BeginPopup("Error"))
		{
			ImGui::Text("Please fill 'Proto dir path' and 'Proto id file path' before enabling capture.");
			ImGui::EndPopup();
		}

		ConfigWidget(m_PipeEnabled, "Enable sending packet data to pipe with name 'genshin_packet_pipe'.\n"\
			"This feature can be used to do external monitoring tool.");
		//ConfigWidget(m_ManipulationEnabled, "Enabling manipulation packet feature, that allows to replace, block incoming/outcoming packets." \
		//	"\nThis feature often needs, to read-write pipe operation, so can decrease network bandwidth.");

		if (m_CapturingEnabled)
		{
			ImGui::Text("This parameters can be changed only when 'Capturing' disabled.");
			ImGui::BeginDisabled();
		}

		ConfigWidget(m_ProtoDirPath, "Path to directory contains genshin .proto files.");
		ConfigWidget(m_ProtoIDFilePath, "Path to json file contained packet id->packet name info.");

		if (m_CapturingEnabled)
			ImGui::EndDisabled();
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

		PacketData packetData = ParseRawPacketData((char*)packet->data, packet->dataLen);
		if (!packetData.valid)
			return true;

		auto name = m_ProtoManager.GetName(packetData.messageId);
		if (!name)
			return true;

		std::cout << magic_enum::enum_name(type) << " Name: " << *name;

		auto message = m_ProtoManager.GetJson(packetData.messageId, packetData.messageData);
		if (!message)
		{
			std::cout << std::endl;
			return true;
		}

		std::cout << " Message: " << *message << std::endl;

		if (!m_PipeEnabled || (!m_Pipe.IsPipeOpened() && !TryConnectToPipe()))
			return true;

		packetData.waitForModifyData = false; // m_ManipulationEnabled;
		packetData.type = type;
		SendData(packetData);

		//if (m_ManipulationEnabled)
		//{
		//	auto modifyData = ReceiveData();
		//	if (modifyData.type == PacketModifyType::Blocked)
		//		return false;

		//	if (modifyData.type == PacketModifyType::Modified)
		//	{
		//		auto dataSize = modifyData.modifiedData.size();
		//		packet->packetData = new byte[dataSize]();
		//		memcpy_s(packet->packetData, dataSize, modifyData.modifiedData.packetData(), dataSize);
		//		packet->dataLen = dataSize;
		//	}
		//}
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
		// Decrypting packetData
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
			//LOG_DEBUG("%s packetData with mid %d.", magic_enum::enum_name(packetData.type).packetData(), packetData.messageId);
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

