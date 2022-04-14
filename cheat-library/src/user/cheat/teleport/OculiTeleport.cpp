#include "pch-il2cpp.h"
#include "OculiTeleport.h"

#include <helpers.h>
#include <cheat/game/util.h>

namespace cheat::feature 
{
    OculiTeleport::OculiTeleport() : ItemTeleportBase("OculiTeleport", "Oculi")
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

	bool OculiTeleport::IsValid(game::Entity* entity) const
	{
		return game::GetFilterCrystalShell().IsValid(entity);
	}

}

