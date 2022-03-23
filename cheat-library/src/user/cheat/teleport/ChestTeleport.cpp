#include "pch-il2cpp.h"
#include "ChestTeleport.h"

#include <helpers.h>
#include <cheat/game.h>

namespace cheat::feature 
{
    static bool ChestFilter(app::BaseEntity* entity) 
    {
        return game::IsEntityFilterValid(entity, game::GetFilterChest());
    }

    ChestTeleport::ChestTeleport() : ItemTeleportBase("ChestTeleport", "Chest", ChestFilter),
        NF(m_OnlyUnlocked, "Show only unlocked chest", "ChestTeleport", true) { }

    void cheat::feature::ChestTeleport::DrawFilterOptions()
    {
        ConfigWidget(m_ShowInfo, "Showing short info about nearest chest to info window.");
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
}

