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
        ConfigWidget(m_ShowInfo, "Showing short info about nearest chest to info window.");

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


	ChestTeleport::ItemType GetItemType(const std::string& entityName)
	{
		if (entityName.find("TreasureBox") != std::string::npos)
			return ChestTeleport::ItemType::Chest;

		if (entityName.find("Search") != std::string::npos ||
			entityName.find("JunkChest") != std::string::npos)
			return ChestTeleport::ItemType::Investigate;

		if (entityName.find("BookPage") != std::string::npos)
			return ChestTeleport::ItemType::BookPage;

		if (entityName.find("BGM") != std::string::npos)
			return ChestTeleport::ItemType::BGM;

		return ChestTeleport::ItemType::None;
	}

	ChestTeleport::ChestRarity GetChestRarity(const std::string& entityName)
	{
		auto rarityIdPos = entityName.find('0');
		if (rarityIdPos == std::string::npos)
			return ChestTeleport::ChestRarity::Unknown;

		int rarityId = entityName[rarityIdPos + 1] - 48;
		switch (rarityId)
		{
		case 1:
			return ChestTeleport::ChestRarity::Common;
		case 2:
			return ChestTeleport::ChestRarity::Exquisite;
		case 4:
			return ChestTeleport::ChestRarity::Precious;
		case 5:
			return ChestTeleport::ChestRarity::Luxurious;
		case 6:
			return ChestTeleport::ChestRarity::Remarkable;
		default:
			return ChestTeleport::ChestRarity::Unknown;
		}
	}

	ChestTeleport::ChestState GetChestState(app::BaseEntity* entity)
	{
		auto chestPlugin = game::GetLCPlugin<app::LCChestPlugin>(entity, *app::LCChestPlugin__TypeInfo);
		if (chestPlugin == nullptr ||
			chestPlugin->fields._owner == nullptr ||
			chestPlugin->fields._owner->fields._dataItem == nullptr)
			return ChestTeleport::ChestState::Invalid;

		auto state = static_cast<app::GadgetState__Enum>(chestPlugin->fields._owner->fields._dataItem->fields.gadgetState);
		switch (state)
		{
		case app::GadgetState__Enum::ChestLocked:
			return ChestTeleport::ChestState::Locked;
		case app::GadgetState__Enum::ChestRock:
			return ChestTeleport::ChestState::InRock;
		case app::GadgetState__Enum::ChestFrozen:
			return ChestTeleport::ChestState::Frozen;
		case app::GadgetState__Enum::ChestBramble:
			return ChestTeleport::ChestState::Bramble;
		case app::GadgetState__Enum::ChestTrap:
			return ChestTeleport::ChestState::Trap;
		case app::GadgetState__Enum::ChestOpened:
			return ChestTeleport::ChestState::Invalid;
		default:
			return ChestTeleport::ChestState::None;
		}
	}

	ImColor GetChestColor(ChestTeleport::ItemType itemType,
		ChestTeleport::ChestRarity rarity = ChestTeleport::ChestRarity::Unknown)
	{
		switch (itemType)
		{
		case ChestTeleport::ItemType::Chest:
		{
			switch (rarity)
			{
			case ChestTeleport::ChestRarity::Common:
				return ImColor(255, 255, 255);
			case ChestTeleport::ChestRarity::Exquisite:
				return ImColor(0, 218, 255);
			case ChestTeleport::ChestRarity::Precious:
				return ImColor(231, 112, 255);
			case ChestTeleport::ChestRarity::Luxurious:
				return ImColor(246, 255, 0);
			case ChestTeleport::ChestRarity::Remarkable:
				return ImColor(255, 137, 0);
			case ChestTeleport::ChestRarity::Unknown:
			default:
				return ImColor(72, 72, 72);
			}
		}
		case ChestTeleport::ItemType::Investigate:
		case ChestTeleport::ItemType::BookPage:
		case ChestTeleport::ItemType::BGM:
			return ImColor(104, 146, 163);
		case ChestTeleport::ItemType::None:
		default:
			return ImColor(72, 72, 72);
		}
	}

	std::string GetChestMinName(ChestTeleport::ItemType itemType,
		ChestTeleport::ChestRarity rarity = ChestTeleport::ChestRarity::Unknown)
	{
		switch (itemType)
		{
		case ChestTeleport::ItemType::Chest:
		{
			switch (rarity)
			{
			case ChestTeleport::ChestRarity::Common:
				return "CR1";
			case ChestTeleport::ChestRarity::Exquisite:
				return "CR2";
			case ChestTeleport::ChestRarity::Precious:
				return "CR3";
			case ChestTeleport::ChestRarity::Luxurious:
				return "CR4";
			case ChestTeleport::ChestRarity::Remarkable:
				return "CR5";
			case ChestTeleport::ChestRarity::Unknown:
			default:
				return "UNK";
			}
		}
		case ChestTeleport::ItemType::Investigate:
			return "INV";
		case ChestTeleport::ItemType::BookPage:
			return "BPG";
		case ChestTeleport::ItemType::BGM:
			return "BGM";
		case ChestTeleport::ItemType::None:
		default:
			return "UNK";
		}
	}

	cheat::feature::ChestTeleport::FilterStatus ChestTeleport::FilterChest(app::BaseEntity* entity)
	{
		if (!game::IsEntityFilterValid(entity, game::GetFilterChest()))
			return FilterStatus::Invalid;

		auto entityName = game::GetEntityName(entity);

		auto itemType = GetItemType(entityName);
		switch (itemType)
		{
		case cheat::feature::ChestTeleport::ItemType::Chest:
		{
			if (!m_FilterChest)
				return FilterStatus::Invalid;
			
			auto chestRarity = GetChestRarity(entityName);
			if (chestRarity == ChestRarity::Unknown)
				return FilterStatus::Unknown;
			
			bool rarityValid = (chestRarity == ChestRarity::Common && m_FilterChestCommon) ||
				(chestRarity == ChestRarity::Exquisite  && m_FilterChestExquisite) ||
				(chestRarity == ChestRarity::Precious   && m_FilterChestPrecious) ||
				(chestRarity == ChestRarity::Luxurious  && m_FilterChestLuxurious) ||
				(chestRarity == ChestRarity::Remarkable && m_FilterChestRemarkable);

			if (!rarityValid)
				return FilterStatus::Invalid;

			auto chestState = GetChestState(entity);
			if (chestState == ChestState::Invalid)
				return FilterStatus::Invalid;

			bool chestStateValid = chestState == ChestState::None ||
				(chestState == ChestState::Locked  && m_FilterChestLocked) ||
				(chestState == ChestState::InRock  && m_FilterChestInRock) ||
				(chestState == ChestState::Frozen  && m_FilterChestFrozen) ||
				(chestState == ChestState::Bramble && m_FilterChestBramble) ||
				(chestState == ChestState::Trap    && m_FilterChestTrap);

			if (!chestStateValid)
				return FilterStatus::Invalid;

			return FilterStatus::Valid;
		}
		case cheat::feature::ChestTeleport::ItemType::Investigate:
			return m_FilterInvestigates ? FilterStatus::Valid : FilterStatus::Invalid;
		case cheat::feature::ChestTeleport::ItemType::BookPage:
			return m_FilterBookPage ? FilterStatus::Valid : FilterStatus::Invalid;
		case cheat::feature::ChestTeleport::ItemType::BGM:
			return m_FilterBGM ? FilterStatus::Valid : FilterStatus::Invalid;
		case cheat::feature::ChestTeleport::ItemType::None:
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
		auto itemType = GetItemType(entityName);
		
		if (itemType == ChestTeleport::ItemType::Chest)
		{
			auto chestRarity = GetChestRarity(entityName);
			auto color = GetChestColor(itemType, chestRarity);
			auto minName = GetChestMinName(itemType, chestRarity);
			ImGui::TextColored(color, "%s", minName.c_str());
		}
		else
		{
			auto color = GetChestColor(itemType);
			auto minName = GetChestMinName(itemType);
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
			auto itemType = GetItemType(entityName);
			
			ImGui::TableNextColumn();
			if (itemType == ItemType::Chest)
			{
				auto chestRarity = GetChestRarity(entityName);
				auto chestState = GetChestState(entity);
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

		ImGui::TextColored(ImColor(255, 165, 0, 255), "Wow, you found unknowns chest names. Means this chest items have no filter yet.\n"
			"Please create issue (if anyone created it before) with copied text, it'll help improve functionality.");

		TextURL("Create issue", "https://github.com/CallowBlack/genshin-cheat/issues/new", false, false);
		
		if (ImGui::Button("Copy to clipboard"))
		{
			ImGui::LogToClipboard();

			ImGui::LogText("Unknown names:\n");
			
			for (auto& entity : unknowns)
				ImGui::LogText("%s\n", game::GetEntityName(entity).c_str());
			
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

