#pragma once
#include <cheat/Feature.h>
#include <common/config/Config.h>

#include <gcclib/PipeTransfer.h>
#include <gcclib/data/PacketData.h>
#include <gcclib/data/PacketModifyData.h>

namespace cheat::feature 
{

	class PacketSniffer : public Feature
    {
	public:
		config::field::BaseField<bool> m_CapturingEnabled;
		config::field::BaseField<bool> m_ManipulationEnabled;

		static PacketSniffer& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override { return false; };
		void DrawStatus() override {};

		virtual bool NeedInfoDraw() const override { return false; };
		void DrawInfo() override {};
	
		bool OnPacketIO(app::KcpPacket_1* packet, PacketType type);

	private:
		
		PipeTransfer m_Pipe;
		std::time_t m_NextTimeToConnect;

		PacketSniffer();
		
		bool TryConnectToPipe();
		PacketData ParseRawPacketData(char* encryptedData, uint32_t length);
		void SendData(PacketData& data);
		PacketModifyData ReceiveData();

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

