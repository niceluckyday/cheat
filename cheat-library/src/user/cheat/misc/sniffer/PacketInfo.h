#pragma once

#include <cheat-base/pipe/PacketData.h>

#include <nlohmann/json.hpp>

namespace cheat::feature::sniffer 
{
	class PacketInfo
	{
	public:
		PacketInfo(PacketData packetData);

		PacketType type() const;
		uint32_t id() const;
		size_t size() const;
		int64_t time() const;
		std::string name() const;
		nlohmann::json object() const;

		void Draw();

	private:
		nlohmann::json m_JObject;
		int64_t m_Time;
		PacketData m_Data;
	};
}


