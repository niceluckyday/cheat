#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/config.h>

namespace cheat::feature 
{
	class NoCD : public Feature
    {
	public:
		config::Field<config::ToggleHotkey> f_InstantBow;
		config::Field<config::ToggleHotkey> f_Ability;
		config::Field<config::ToggleHotkey> f_Sprint;

		static NoCD& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override;
		void DrawStatus() override;
	
	private:
		NoCD();
	};
}

