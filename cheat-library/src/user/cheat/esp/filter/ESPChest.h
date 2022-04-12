#pragma once


#include <cheat/esp/ESPSectionBase.h>

namespace cheat::feature::esp
{
	class ChestFilter : public IEntityFilter
	{
	public:
		ChestFilter(game::chest::ChestRarity rarity);
		bool IsValid(app::BaseEntity* entity) override;
	
	private:
		game::chest::ChestRarity m_Rarity;
	};

	class ESPChest : public ESPSectionBase
	{
	public:
		static ESPChest& GetInstance();

	private:
		ESPChest();
	};
}


