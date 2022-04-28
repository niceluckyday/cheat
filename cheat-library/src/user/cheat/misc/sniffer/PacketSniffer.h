// ReSharper disable All
#pragma once

#include <filesystem>

#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/config.h>

#include <cheat-base/PipeTransfer.h>
#include <cheat-base/pipe/PacketData.h>
#include <cheat-base/pipe/PacketModifyData.h>

#include "ProtoManager.h"
#include "PacketInfo.h"

namespace cheat::feature 
{
	
	class PacketSniffer : public Feature
    {
	public:
		config::Field<bool> m_CapturingEnabled;
		config::Field<bool> m_ManipulationEnabled;
		config::Field<bool> m_PipeEnabled;
		config::Field<std::string> m_ProtoDirPath;
		config::Field<std::string> m_ProtoIDFilePath;

		static PacketSniffer& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		void DrawExternal() final;
	
		bool OnPacketIO(app::KcpPacket_1* packet, PacketType type);

	private:

		sniffer::ProtoManager m_ProtoManager;
		PipeTransfer m_Pipe;
		std::time_t m_NextTimeToConnect;

		PacketSniffer();
		
		bool TryConnectToPipe();
		PacketData ParseRawPacketData(char* encryptedData, uint32_t length);
		void SendData(PacketData& data);
		PacketModifyData ReceiveData();
		void ProcessUnionMessage(const PacketData& packetData);
		bool OnCapturingChanged();

		static char* EncryptXor(void* content, uint32_t length);
		static bool isLittleEndian();

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
	};
}

