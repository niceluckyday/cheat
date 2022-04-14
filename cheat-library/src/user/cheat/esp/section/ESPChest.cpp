#include "pch-il2cpp.h"
#include "ESPChest.h"

#include <cheat/game/Chest.h>

namespace cheat::feature::esp
{

	ESPChest& ESPChest::GetInstance()
	{
		static ESPChest instance;
		return instance;
	}

	ESPChest::ESPChest() : ESPSectionBase("Chest")
	{
		AddFilter("Common chest",     new ChestFilter(game::Chest::ChestRarity::Common));
		AddFilter("Exquisite chest",  new ChestFilter(game::Chest::ChestRarity::Exquisite));
		AddFilter("Precious chest",   new ChestFilter(game::Chest::ChestRarity::Precious));
		AddFilter("Luxurious chest",  new ChestFilter(game::Chest::ChestRarity::Luxurious));
		AddFilter("Remarkable chest", new ChestFilter(game::Chest::ChestRarity::Remarkable));
	}

	bool ChestFilter::IsValid(game::Entity* entity) const
	{
		if (!entity->isChest())
			return false;

		game::Chest* chest = reinterpret_cast<game::Chest*>(entity);
		return chest->itemType() == game::Chest::ItemType::Chest && chest->chestRarity() == m_Rarity;
	}

	ChestFilter::ChestFilter(game::Chest::ChestRarity rarity) : m_Rarity(rarity)
	{

	}

}
