#include "pch-il2cpp.h"
#include "ItemTeleportBase.h"

#include <helpers.h>
#include <cheat/teleport/MapTeleport.h>
#include <cheat-base/cheat/CheatManager.h>
#include <cheat/events.h>

namespace cheat::feature 
{
    ItemTeleportBase::ItemTeleportBase(const std::string& section, const std::string& name, game::FilterFunc filter) : Feature(),
		NF(m_Key, "TP to nearest key", section, Hotkey()),
		NF(m_ShowInfo, "Show info", section, true),
        section(section), name(name), filter(filter)
    {
		events::KeyUpEvent += MY_METHOD_HANDLER(ItemTeleportBase::OnKeyUp);
    }

    void ItemTeleportBase::DrawMain()
    {
		auto desc = util::string_format("When key pressed, will teleport to nearest %s if exists.", name.c_str());
		ConfigWidget(m_Key, desc.c_str());

		DrawFilterOptions();

		auto nodeName = util::string_format("%s list", name.c_str());
		if (ImGui::TreeNode(nodeName.c_str()))
		{
			DrawEntities(filter);
			ImGui::TreePop();
		}
    }

	bool ItemTeleportBase::NeedInfoDraw() const
{
		return m_ShowInfo;
	}

	void ItemTeleportBase::DrawInfo()
	{
		DrawNearestEntityInfo(name.c_str(), filter);
	}

	void ItemTeleportBase::OnKeyUp(short key, bool& cancelled)
	{
		if (CheatManager::IsMenuShowed())
			return;

		if (m_Key.value().IsPressed(key))
		{
			auto entity = game::FindNearestEntity(filter);
			if (entity != nullptr)
			{
				MapTeleport& mapTeleport = MapTeleport::GetInstance();
				mapTeleport.TeleportTo(game::GetAbsolutePosition(entity));
			}
		}
	}

	void ItemTeleportBase::DrawNearestEntityInfo(const char* prefix, game::FilterFunc filter)
	{
		auto nearestEntity = game::FindNearestEntity(filter);
		if (nearestEntity == nullptr)
		{
			ImGui::Text(prefix); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.1f, 0.1f, 1.0f), "not found");
			return;
		}
		ImGui::Text(prefix); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), "%.3fm", game::GetDistToAvatar(nearestEntity));
	}

	void ItemTeleportBase::DrawEntities(const game::FilterFunc& filter)
	{
		auto entities = game::FindEntities(filter);
		if (entities.size() == 0)
		{
			ImGui::Text("Not found.");
			return;
		}

		for (const auto& entity : entities)
		{
			ImGui::Text("Dist %.03fm", game::GetDistToAvatar(entity));
			ImGui::SameLine();
			auto label = util::string_format("Teleport ## %p", entity);
			if (ImGui::Button(label.c_str()))
			{
				MapTeleport& mapTeleport = MapTeleport::GetInstance();
				mapTeleport.TeleportTo(game::GetAbsolutePosition(entity));
			}
		}
	}
}

