#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>

namespace cheat::feature 
{

	class KillAura : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::BaseField<bool> m_OnlyTargeted;
		config::field::BaseField<float> m_Range;
		config::field::BaseField<int> m_AttackDelay;
		config::field::BaseField<int> m_RepeatDelay;

		static KillAura& GetInstance();

		void OnGameUpdate();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override;
		void DrawStatus() override;
	
	private:
		KillAura();
	};
}

