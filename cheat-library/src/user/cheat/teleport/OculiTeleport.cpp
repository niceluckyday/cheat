#include "pch-il2cpp.h"
#include "OculiTeleport.h"

#include <imgui.h>
#include <common/util.h>
#include <helpers.h>
#include <gui/gui-util.h>

namespace cheat::feature 
{
    static bool OculiFilter(app::BaseEntity* entity) 
    {
        return IsEntityFilterValid(entity, GetFilterCrystalShell());
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

