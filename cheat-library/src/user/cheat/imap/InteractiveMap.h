#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>

namespace cheat::feature 
{

	class InteractiveMap : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::BaseField<float> m_IconSize;

		static InteractiveMap& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		void DrawExternal() override;
	
	private:
		InteractiveMap();
	};
}

