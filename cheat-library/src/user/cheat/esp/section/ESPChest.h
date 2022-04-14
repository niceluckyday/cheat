#pragma once

#include <cheat/game/IEntityFilter.h>
#include <cheat/esp/ESPSectionBase.h>

#include <cheat/game/Chest.h>

namespace cheat::feature::esp
{
	class ChestFilter : public game::IEntityFilter
	{
	public:
		ChestFilter(game::Chest::ChestRarity rarity);
		bool IsValid(game::Entity* entity) const override;
	
	private:
		game::Chest::ChestRarity m_Rarity;
	};

	class ESPChest : public ESPSectionBase
	{
	public:
		static ESPChest& GetInstance();

	private:
		ESPChest();
	};
}


