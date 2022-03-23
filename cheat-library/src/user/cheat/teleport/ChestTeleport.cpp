#include "pch-il2cpp.h"
#include "ChestTeleport.h"

#include <imgui.h>
#include <common/util.h>
#include <helpers.h>
#include <gui/gui-util.h>

namespace cheat::feature 
{
    static bool OculiFilter(app::BaseEntity* entity) 
    {
        return IsEntityFilterValid(entity, GetFilterChest());
    }

    ChestTeleport::ChestTeleport() : ItemTeleportBase("ChestTeleport", "Chest", OculiFilter),
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

