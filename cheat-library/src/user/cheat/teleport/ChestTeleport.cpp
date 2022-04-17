#include "pch-il2cpp.h"
#include "ChestTeleport.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/Chest.h>
#include <cheat/game/util.h>
#include <cheat/teleport/MapTeleport.h>

namespace cheat::feature 
{

    ChestTeleport::ChestTeleport() : ItemTeleportBase("ChestTeleport", "Chest"),
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

		NF(m_FilterChest          , "Chests",       "ChestTeleport", true),
		NF(m_FilterInvestigates   , "Investigates", "ChestTeleport", false),
		NF(m_FilterBookPage       , "Book pages",   "ChestTeleport", false),
		NF(m_FilterBGM            , "BGMs",         "ChestTeleport", false),
		NF(m_FilterQuestInt       , "Quest Interaction",  "ChestTeleport", false),
		NF(m_FilterFloraChest     , "Flora chest",  "ChestTeleport", false),

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
			ConfigWidget(m_FilterQuestInt);
			ConfigWidget(m_FilterFloraChest);
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

	bool ChestTeleport::IsValid(game::Entity* entity) const
	{
		if (!entity->isChest())
			return false;

		auto chest = reinterpret_cast<game::Chest*>(entity);
		auto filterResult = FilterChest(chest);
		return filterResult == ChestTeleport::FilterStatus::Valid ||
			(filterResult == ChestTeleport::FilterStatus::Unknown && m_FilterUnknown);
	}

	cheat::feature::ChestTeleport::FilterStatus ChestTeleport::FilterChest(game::Chest* entity) const
	{
		auto itemType = entity->itemType();
		switch (itemType)
		{
		case game::Chest::ItemType::Chest:
		{
			if (!m_FilterChest)
				return FilterStatus::Invalid;
			
			auto chestRarity = entity->chestRarity();
			if (chestRarity == game::Chest::ChestRarity::Unknown)
				return FilterStatus::Unknown;
			
			bool rarityValid = (chestRarity == game::Chest::ChestRarity::Common && m_FilterChestCommon) ||
				(chestRarity == game::Chest::ChestRarity::Exquisite  && m_FilterChestExquisite) ||
				(chestRarity == game::Chest::ChestRarity::Precious   && m_FilterChestPrecious) ||
				(chestRarity == game::Chest::ChestRarity::Luxurious  && m_FilterChestLuxurious) ||
				(chestRarity == game::Chest::ChestRarity::Remarkable && m_FilterChestRemarkable);

			if (!rarityValid)
				return FilterStatus::Invalid;

			auto chestState = entity->chestState();
			if (chestState == game::Chest::ChestState::Invalid)
				return FilterStatus::Invalid;

			bool chestStateValid = chestState == game::Chest::ChestState::None ||
				(chestState == game::Chest::ChestState::Locked  && m_FilterChestLocked) ||
				(chestState == game::Chest::ChestState::InRock  && m_FilterChestInRock) ||
				(chestState == game::Chest::ChestState::Frozen  && m_FilterChestFrozen) ||
				(chestState == game::Chest::ChestState::Bramble && m_FilterChestBramble) ||
				(chestState == game::Chest::ChestState::Trap    && m_FilterChestTrap);

			if (!chestStateValid)
				return FilterStatus::Invalid;

			return FilterStatus::Valid;
		}
		case game::Chest::ItemType::Investigate:
			return m_FilterInvestigates ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::Chest::ItemType::BookPage:
			return m_FilterBookPage ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::Chest::ItemType::BGM:
			return m_FilterBGM ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::Chest::ItemType::QuestInteract:
			return m_FilterQuestInt ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::Chest::ItemType::Flora:
			return m_FilterFloraChest ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::Chest::ItemType::None:
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
		auto entity = game::FindNearestEntity(*this);
		auto chest = reinterpret_cast<game::Chest*>(entity);

		DrawEntityInfo(entity);
		if (entity == nullptr)
			return;
		ImGui::SameLine();

		ImGui::TextColored(chest->chestColor(), "%s", chest->minName());
	}

	void ChestTeleport::DrawChests()
	{	
		if (!ImGui::TreeNode("Items"))
			return;

		auto& manager = game::EntityManager::instance();
		auto entities = manager.entities(*this);

		ImGui::BeginTable("ChestsTable", 2);
		for (auto& entity : entities)
		{
			ImGui::PushID(entity);
			auto chest = reinterpret_cast<game::Chest*>(entity);

			ImGui::TableNextColumn();
			if (chest->itemType() == game::Chest::ItemType::Chest)
			{
				ImGui::TextColored(chest->chestColor(), "%s [%s] [%s] at %0.3fm", 
					magic_enum::enum_name(chest->itemType()).data(),
					magic_enum::enum_name(chest->chestRarity()).data(),
					magic_enum::enum_name(chest->chestState()).data(),
					manager.avatar()->distance(entity));
			}
			else
			{
				ImGui::TextColored(chest->chestColor(), "%s at %0.3fm", magic_enum::enum_name(chest->itemType()).data(), 
					manager.avatar()->distance(entity));
			}

			ImGui::TableNextColumn();

			if (ImGui::Button("Teleport"))
			{
				auto& mapTeleport = MapTeleport::GetInstance();
				mapTeleport.TeleportTo(chest->absolutePosition());
			}
			ImGui::PopID();
		}
		ImGui::EndTable();
		ImGui::TreePop();
	}

	static bool ChestUnknownFilter(game::Entity* entity)
	{
		if (!entity->isChest())
			return false;

		auto chest = reinterpret_cast<game::Chest*>(entity);
		auto& chestTp = ChestTeleport::GetInstance();
		return chestTp.FilterChest(chest) == ChestTeleport::FilterStatus::Unknown;
	}

	void ChestTeleport::DrawUnknowns()
	{
		auto& manager = game::EntityManager::instance();
		auto unknowns = manager.entities(ChestUnknownFilter);
		if (unknowns.empty())
			return;

		ImGui::TextColored(ImColor(255, 165, 0, 255), "Wow, you found unknowns chest names. Means this chest items have no filter yet.");
		TextURL("Please check issue on github with information about it.", "https://github.com/CallowBlack/genshin-cheat/issues/48", false, false);
		
		if (ImGui::Button("Copy to clipboard"))
		{
			ImGui::LogToClipboard();

			ImGui::LogText("Unknown names:\n");
			
			for (auto& entity : unknowns)
				ImGui::LogText("%s; position: %s; scene: %u\n", entity->name().c_str(), 
					il2cppi_to_string(entity->relativePosition()).c_str(), game::GetCurrentPlayerSceneID());
			
			ImGui::LogFinish();
		}

		if (!ImGui::TreeNode("Unknown items"))
			return;

		ImGui::BeginTable("UnknownsTable", 2);

		for (auto& entity : unknowns)
		{
			ImGui::PushID(entity);
			
			ImGui::TableNextColumn();
			ImGui::Text("%s. Dist %0.3f", entity->name().c_str(), manager.avatar()->distance(entity));
			
			ImGui::TableNextColumn();
			if (ImGui::Button("TP"))
			{
				auto& mapTeleport = MapTeleport::GetInstance();
				mapTeleport.TeleportTo(entity->absolutePosition());
			}

			ImGui::PopID();
		}
		ImGui::EndTable();
		ImGui::TreePop();
	}



}

