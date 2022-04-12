#include "pch-il2cpp.h"
#include "ESPChest.h"

namespace cheat::feature::esp
{

	ESPChest& ESPChest::GetInstance()
	{
		static ESPChest instance;
		return instance;
	}

	ESPChest::ESPChest() : ESPSectionBase("Chest")
	{
		AddFilter("Common chest", new ChestFilter(game::chest::ChestRarity::Common));
		AddFilter("Exquisite chest", new ChestFilter(game::chest::ChestRarity::Exquisite));
		AddFilter("Precious chest", new ChestFilter(game::chest::ChestRarity::Precious));
		AddFilter("Luxurious chest", new ChestFilter(game::chest::ChestRarity::Luxurious));
		AddFilter("Remarkable chest", new ChestFilter(game::chest::ChestRarity::Remarkable));
	}

	bool ChestFilter::IsValid(app::BaseEntity* entity)
	{
		// return entity->fields.entityType == app::EntityType__Enum_1::GatherObject;

		auto name = game::GetEntityName(entity);
		return game::chest::GetItemType(name) == game::chest::ItemType::Chest &&
			game::chest::GetChestRarity(name) == m_Rarity;
	}

	ChestFilter::ChestFilter(game::chest::ChestRarity rarity) : m_Rarity(rarity)
	{

	}

}
