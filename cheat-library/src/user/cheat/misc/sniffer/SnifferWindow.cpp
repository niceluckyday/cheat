#include "pch-il2cpp.h"
#include "SnifferWindow.h"

namespace cheat::feature::sniffer
{
	SnifferWindow& SnifferWindow::GetInstance()
	{
		static SnifferWindow instance;
		return instance;
	}

	static auto nameCompare = [](const PacketInfo& a, const PacketInfo& b) { return a.name() < b.name(); };
	static auto sizeCompare = [](const PacketInfo& a, const PacketInfo& b) { return a.size() < b.size(); };
	static auto idCompare =   [](const PacketInfo& a, const PacketInfo& b) { return a.id() < b.id(); };
	static auto typeCompare = [](const PacketInfo& a, const PacketInfo& b) { return a.type() < b.type(); };
	static auto timeCompare = [](const PacketInfo& a, const PacketInfo& b) { return a.time() < b.time(); };

	void SnifferWindow::OnPacketIO(const PacketInfo& info)
	{
		if (m_SortValue == SortValue::Time)
		{
			m_CapturedPackets.push_back(info);
			return;
		}

		switch (m_SortValue)
		{
		case cheat::feature::sniffer::SnifferWindow::SortValue::Name:
			m_CapturedPackets.insert(std::lower_bound(m_CapturedPackets.begin(), m_CapturedPackets.end(), info, nameCompare), info);
			break;
		case cheat::feature::sniffer::SnifferWindow::SortValue::Size:
			m_CapturedPackets.insert(std::lower_bound(m_CapturedPackets.begin(), m_CapturedPackets.end(), info, sizeCompare), info);
			break;
		case cheat::feature::sniffer::SnifferWindow::SortValue::Id:
			m_CapturedPackets.insert(std::lower_bound(m_CapturedPackets.begin(), m_CapturedPackets.end(), info, idCompare), info);
			break;
		case cheat::feature::sniffer::SnifferWindow::SortValue::Type:
			m_CapturedPackets.insert(std::lower_bound(m_CapturedPackets.begin(), m_CapturedPackets.end(), info, typeCompare), info);
			break;
		default:
			break;
		}
	}

	template <typename T>
	static bool ComboEnum(const char* label, T* currentValue)
	{
		auto name = magic_enum::enum_name(*currentValue);
		auto& current = *currentValue;
		bool result = false;
		if (ImGui::BeginCombo(label, name.data()))
		{
			for (auto& entry : magic_enum::enum_entries<T>())
			{
				bool is_selected = (name == entry.second);
				if (ImGui::Selectable(entry.second.data(), is_selected))
				{
					current = entry.first;
					result = true;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		return result;
	}

	void SnifferWindow::Draw()
	{

		if (!ImGui::Begin("Packet sniffer", m_Show))
			return;

		BeginGroupPanel("Sort", ImVec2(-1, 0));
		if (ComboEnum("Value", &m_SortValue))
		{
			switch (m_SortValue)
			{
			case cheat::feature::sniffer::SnifferWindow::SortValue::Name:
				m_CapturedPackets.sort(nameCompare);
				break;
			case cheat::feature::sniffer::SnifferWindow::SortValue::Size:
				m_CapturedPackets.sort(sizeCompare);
				break;
			case cheat::feature::sniffer::SnifferWindow::SortValue::Id:
				m_CapturedPackets.sort(idCompare);
				break;
			case cheat::feature::sniffer::SnifferWindow::SortValue::Type:
				m_CapturedPackets.sort(typeCompare);
				break;
			case cheat::feature::sniffer::SnifferWindow::SortValue::Time:
				m_CapturedPackets.sort(timeCompare);
				break;
			default:
				break;
			}
		}
		ComboEnum("Type", &m_SortType);
		EndGroupPanel();

		if (ImGui::Button("Clear history"))
		{
			m_CapturedPackets.clear();
		}

		ImGui::BeginChild("Packets");

		if (m_SortType == SortType::Asc)
		{
			for (auto& info : m_CapturedPackets)
			{
				info.Draw();
			}
		}
		else
		{
			for (auto it = m_CapturedPackets.rbegin(); it != m_CapturedPackets.rend(); it++)
			{
				it->Draw();
			}
		}
		ImGui::EndChild();

		ImGui::End();
	}

	SnifferWindow::SnifferWindow() :
		NF(m_Show, "Show capturing window", "SnifferWindow", false)
	{

	}

	void Filter::Draw()
	{

	}

	bool Filter::Execute(const sniffer::PacketInfo& info)
	{

	}

	nlohmann::json Filter::Serialize()
	{

	}

	void FilterGroup::Draw()
	{

	}

	bool FilterGroup::Execute(const sniffer::PacketInfo& info)
	{

	}

	void FilterGroup::AddFilter(const Filter& filter)
	{

	}

	void FilterGroup::RemoveFilter(const Filter& filter)
	{

	}

	void FilterGroup::SetRule(Rule rule)
	{

	}

	nlohmann::json FilterGroup::Serialize()
	{

	}

}
