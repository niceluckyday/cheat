#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>

#include <il2cpp-appdata.h>

namespace cheat::feature 
{

	class RapidFire : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::BaseField<int> m_Multiplier;
		config::field::BaseField<bool> m_OnePunch;

		static RapidFire& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override;
		void DrawStatus() override;

		virtual bool NeedInfoDraw() const override { return false; };
		void DrawInfo() override {};
	
		int GetAttackCount(app::LCBaseCombat* combat, uint32_t targetID, app::AttackResult* attackResult);
	private:
		RapidFire();
		int CalcCountToKill(float attackDamage, uint32_t targetID);
	};
}

