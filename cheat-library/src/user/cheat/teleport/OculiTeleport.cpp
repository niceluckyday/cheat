#include "pch-il2cpp.h"
#include "OculiTeleport.h"

#include <helpers.h>
#include <cheat/game.h>

namespace cheat::feature 
{
    static bool OculiFilter(app::BaseEntity* entity) 
    {
        return game::IsEntityFilterValid(entity, game::GetFilterCrystalShell());
    }

    OculiTeleport::OculiTeleport() : ItemTeleportBase("OculiTeleport", "Oculi", OculiFilter)
    { }

	const FeatureGUIInfo& OculiTeleport::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "0culi teleport", "Teleport", true };
		return info;
	}

    OculiTeleport& OculiTeleport::GetInstance()
	{
		static OculiTeleport instance;
		return instance;
	}
}

