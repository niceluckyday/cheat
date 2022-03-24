#pragma once
#include "PacketInfo.h"

namespace cheat::feature::sniffer
{
	class Filter
	{
	public:
		enum class CompareType
		{
			Regex, Equal, Contains, Less, LessEqual, More, MoreEqual
		};

		enum class ObjectType
		{
			KeyValue, AnyKey, AnyValue
		};

		void Draw();
		bool Execute(const sniffer::PacketInfo& info);

		nlohmann::json Serialize();
	};

	class FilterGroup
	{
	public:

		enum class Rule
		{
			AND, OR, NOT
		};

		void Draw();
		bool Execute(const sniffer::PacketInfo& info);

		void AddFilter(const Filter& filter);
		void RemoveFilter(const Filter& filter);

		void SetRule(Rule rule);

		nlohmann::json Serialize();
	};

	class SnifferWindow
	{
	public:
		config::field::BaseField<bool> m_Show;

		SnifferWindow(SnifferWindow const&) = delete;
		void operator=(SnifferWindow const&) = delete;

		static SnifferWindow& GetInstance();
		void OnPacketIO(const PacketInfo& info);
		void Draw();

	private:
		enum class SortValue
		{
			Time, Name, Size, Id, Type
		};

		enum class SortType
		{
			Desc, Asc
		};

		SortValue m_SortValue;
		SortType m_SortType;

		std::list<sniffer::PacketInfo> m_CapturedPackets;
		FilterGroup m_FilterGroup;

		SnifferWindow();
	};
}

