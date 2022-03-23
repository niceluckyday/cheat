#pragma once
#include "ItemTeleportBase.h"

namespace cheat::feature 
{

	class ChestTeleport : public ItemTeleportBase
    {
	public:
		config::field::BaseField<bool> m_OnlyUnlocked;

		static ChestTeleport& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		virtual void DrawFilterOptions() final;
	
	private:
		ChestTeleport();
	};
}

