#pragma once
#include "ItemTeleportBase.h"

namespace cheat::feature 
{

	class OculiTeleport : public ItemTeleportBase
    {
	public:
		static OculiTeleport& GetInstance();
		const FeatureGUIInfo& GetGUIInfo() const override;
	private:
		OculiTeleport();
	};
}

