#include "pch-il2cpp.h"
#include "ChestTeleport.h"

#include <helpers.h>
#include <cheat/game.h>
#include <cheat/teleport/MapTeleport.h>

namespace cheat::feature 
{
    static bool ChestFilter(app::BaseEntity* entity) 
    {
		auto& chestTp = ChestTeleport::GetInstance();
		auto filterResult = chestTp.FilterChest(entity);
        return filterResult == ChestTeleport::FilterStatus::Valid ||
			(filterResult == ChestTeleport::FilterStatus::Unknown && chestTp.m_FilterUnknown);
    }

	static bool ChestUnknownFilter(app::BaseEntity* entity)
	{
		auto& chestTp = ChestTeleport::GetInstance();
		return chestTp.FilterChest(entity) == ChestTeleport::FilterStatus::Unknown;
	}

    ChestTeleport::ChestTeleport() : ItemTeleportBase("ChestTeleport", "Chest", ChestFilter),
        NF(m_FilterChestLocked    , "Locked",       "ChestTeleport", true),
		NF(m_FilterChestInRock    , "In rock",      "ChestTeleport", true),
		NF(m_FilterChestFrozen    , "Frozen",       "ChestTeleport", true),
		NF(m_FilterChestBramble   , "Bramble",      "ChestTeleport", true),
		NF(m_FilterChestTrap      , "Trap",         "ChestTeleport", true),

		NF(m_FilterChestCommon    , "Common",       "ChestTeleport", true),
		NF(m_FilterChestExquisite , "Exquisite",    "ChestTeleport", true),
		NF(m_FilterChestPrecious  , "Precious",     "ChestTeleport", true),
		NF(m_FilterChestLuxurious , "Luxurious",    "ChestTeleport", true),
		NF(m_FilterChestRemarkable, "Remarkable",   "ChestTeleport", true),

		NF(m_FilterChest          , "Chests",        "ChestTeleport", true),
		NF(m_FilterInvestigates   , "Investigates", "ChestTeleport", true),
		NF(m_FilterBookPage       , "Book pages",   "ChestTeleport", true),
		NF(m_FilterBGM            , "BGMs",         "ChestTeleport", true),

		NF(m_FilterUnknown        , "Unknowns",     "ChestTeleport", true)
	{ }


    void cheat::feature::ChestTeleport::DrawFilterOptions()
    {
		ConfigWidget(m_ShowInfo, "Show short info about nearest chest to the info window.");

		if (ImGui::TreeNode("Filters"))
		{

			ImGui::Text("Type filters");

			ConfigWidget(m_FilterChest, "Detecting chests. Also unlock chest filters.");
			ConfigWidget(m_FilterInvestigates);
			ConfigWidget(m_FilterBookPage);
			ConfigWidget(m_FilterBGM);
			ConfigWidget(m_FilterUnknown, "Detecting items, what is not filtered.\nYou can see these items below, if they exists.");

			ImGui::Spacing();

			if (!m_FilterChest)
				ImGui::BeginDisabled();

			if (ImGui::BeginTable("Chest filters", 2, ImGuiTableFlags_NoBordersInBody))
			{
				ImGui::TableNextColumn();
				ImGui::Text("Rarity filter");

				ImGui::TableNextColumn();
				ImGui::Text("State filter");

				ImGui::TableNextColumn();
				ConfigWidget(m_FilterChestCommon);

				ImGui::TableNextColumn();
				ConfigWidget(m_FilterChestLocked);


				ImGui::TableNextColumn();
				ConfigWidget(m_FilterChestExquisite);

				ImGui::TableNextColumn();
				ConfigWidget(m_FilterChestInRock);


				ImGui::TableNextColumn();
				ConfigWidget(m_FilterChestPrecious);

				ImGui::TableNextColumn();
				ConfigWidget(m_FilterChestFrozen);


				ImGui::TableNextColumn();
				ConfigWidget(m_FilterChestLuxurious);

				ImGui::TableNextColumn();
				ConfigWidget(m_FilterChestBramble);


				ImGui::TableNextColumn();
				ConfigWidget(m_FilterChestRemarkable);

				ImGui::TableNextColumn();
				ConfigWidget(m_FilterChestTrap);

				ImGui::EndTable();
			}

			if (!m_FilterChest)
				ImGui::EndDisabled();

			ImGui::TreePop();
		}
    }

	const FeatureGUIInfo& ChestTeleport::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "Chest teleport", "Teleport", true };
		return info;
	}

    ChestTeleport& ChestTeleport::GetInstance()
	{
		static ChestTeleport instance;
		return instance;
	}

	cheat::feature::ChestTeleport::FilterStatus ChestTeleport::FilterChest(app::BaseEntity* entity)
	{
		if (!game::IsEntityFilterValid(entity, game::GetFilterChest()))
			return FilterStatus::Invalid;

		auto entityName = game::GetEntityName(entity);

		auto itemType = game::chest::GetItemType(entityName);
		switch (itemType)
		{
		case game::chest::ItemType::Chest:
		{
			if (!m_FilterChest)
				return FilterStatus::Invalid;
			
			auto chestRarity = game::chest::GetChestRarity(entityName);
			if (chestRarity == game::chest::ChestRarity::Unknown)
				return FilterStatus::Unknown;
			
			bool rarityValid = (chestRarity == game::chest::ChestRarity::Common && m_FilterChestCommon) ||
				(chestRarity == game::chest::ChestRarity::Exquisite  && m_FilterChestExquisite) ||
				(chestRarity == game::chest::ChestRarity::Precious   && m_FilterChestPrecious) ||
				(chestRarity == game::chest::ChestRarity::Luxurious  && m_FilterChestLuxurious) ||
				(chestRarity == game::chest::ChestRarity::Remarkable && m_FilterChestRemarkable);

			if (!rarityValid)
				return FilterStatus::Invalid;

			auto chestState = game::chest::GetChestState(entity);
			if (chestState == game::chest::ChestState::Invalid)
				return FilterStatus::Invalid;

			bool chestStateValid = chestState == game::chest::ChestState::None ||
				(chestState == game::chest::ChestState::Locked  && m_FilterChestLocked) ||
				(chestState == game::chest::ChestState::InRock  && m_FilterChestInRock) ||
				(chestState == game::chest::ChestState::Frozen  && m_FilterChestFrozen) ||
				(chestState == game::chest::ChestState::Bramble && m_FilterChestBramble) ||
				(chestState == game::chest::ChestState::Trap    && m_FilterChestTrap);

			if (!chestStateValid)
				return FilterStatus::Invalid;

			return FilterStatus::Valid;
		}
		case game::chest::ItemType::Investigate:
			return m_FilterInvestigates ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::chest::ItemType::BookPage:
			return m_FilterBookPage ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::chest::ItemType::BGM:
			return m_FilterBGM ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::chest::ItemType::None:
		default:
			return FilterStatus::Unknown;
		}

		return FilterStatus::Unknown;
	}

	void ChestTeleport::DrawItems()
	{
		DrawUnknowns();
		DrawChests();
	}

	bool ChestTeleport::NeedInfoDraw() const
	{
		return true;
	}

	void ChestTeleport::DrawInfo()
	{
		auto entity = game::FindNearestEntity(ChestFilter);
		DrawEntityInfo(entity);
		if (entity == nullptr)
			return;
		ImGui::SameLine();

		auto entityName = game::GetEntityName(entity);
		auto itemType = game::chest::GetItemType(entityName);
		
		if (itemType == game::chest::ItemType::Chest)
		{
			auto chestRarity = game::chest::GetChestRarity(entityName);
			auto color = game::chest::GetChestColor(itemType, chestRarity);
			auto minName = game::chest::GetChestMinName(itemType, chestRarity);
			ImGui::TextColored(color, "%s", minName.c_str());
		}
		else
		{
			auto color = game::chest::GetChestColor(itemType);
			auto minName = game::chest::GetChestMinName(itemType);
			ImGui::TextColored(color, "%s", minName.c_str());
		}
	}

	void ChestTeleport::DrawChests()
	{
		auto chests = game::FindEntities(ChestFilter);
		
		if (!ImGui::TreeNode("Items"))
			return;

		ImGui::BeginTable("ChestsTable", 2);
		for (auto& entity : chests)
		{
			ImGui::PushID(entity);
			auto entityName = game::GetEntityName(entity);
			auto itemType = game::chest::GetItemType(entityName);
			
			ImGui::TableNextColumn();
			if (itemType == game::chest::ItemType::Chest)
			{
				auto chestRarity = game::chest::GetChestRarity(entityName);
				auto chestState = game::chest::GetChestState(entity);
				auto color = GetChestColor(itemType, chestRarity);
				ImGui::TextColored(color, "%s [%s] [%s] at %0.3fm", 
					magic_enum::enum_name(itemType).data(),
					magic_enum::enum_name(chestRarity).data(),
					magic_enum::enum_name(chestState).data(),
					game::GetDistToAvatar(entity));
			}
			else
			{
				auto color = GetChestColor(itemType);
				ImGui::TextColored(color, "%s at %0.3fm", magic_enum::enum_name(itemType).data(), game::GetDistToAvatar(entity));
			}

			ImGui::TableNextColumn();

			if (ImGui::Button("Teleport"))
			{
				auto& mapTeleport = MapTeleport::GetInstance();
				mapTeleport.TeleportTo(game::GetAbsolutePosition(entity));
			}
			ImGui::PopID();
		}
		ImGui::EndTable();
		ImGui::TreePop();
	}

	void ChestTeleport::DrawUnknowns()
	{
		auto unknowns = game::FindEntities(ChestUnknownFilter);
		if (unknowns.empty())
			return;

		ImGui::TextColored(ImColor(255, 165, 0, 255), "Wow, you found unknowns chest names. Means this chest items have no filter yet.");
		TextURL("Please check issue on github with information about it.", "https://github.com/CallowBlack/genshin-cheat/issues/48", false, false);
		
		if (ImGui::Button("Copy to clipboard"))
		{
			ImGui::LogToClipboard();

			ImGui::LogText("Unknown names:\n");
			
			for (auto& entity : unknowns)
				ImGui::LogText("%s; position: %s\n", game::GetEntityName(entity).c_str(), il2cppi_to_string(game::GetRelativePosition(entity)).c_str());
			
			ImGui::LogFinish();
		}

		if (!ImGui::TreeNode("Unknown items"))
			return;

		ImGui::BeginTable("UnknownsTable", 2);

		for (auto& entity : unknowns)
		{
			ImGui::PushID(entity);
			
			ImGui::TableNextColumn();
			ImGui::Text("%s. Dist %0.3f", game::GetEntityName(entity).c_str(), game::GetDistToAvatar(entity));
			
			ImGui::TableNextColumn();
			if (ImGui::Button("TP"))
			{
				auto& mapTeleport = MapTeleport::GetInstance();
				mapTeleport.TeleportTo(game::GetAbsolutePosition(entity));
			}

			ImGui::PopID();
		}
		ImGui::EndTable();
		ImGui::TreePop();
	}

}

