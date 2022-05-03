#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/config.h>

namespace cheat::feature 
{

	class DialogSkip : public Feature
    {
	public:
		config::Field<config::ToggleHotkey> f_Enabled;
		config::Field<config::ToggleHotkey> f_AutoSelectDialog;
		config::Field<config::ToggleHotkey> f_ExcludeImportant;
		config::Field<config::ToggleHotkey> f_FastDialog;
		config::Field<float> f_TimeSpeedup;

		static DialogSkip& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override;
		void DrawStatus() override;

		void OnCutScenePageUpdate(app::InLevelCutScenePageContext* context);
	
	private:
		DialogSkip();
	};
}

