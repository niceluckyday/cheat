#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>

namespace cheat::feature 
{

	class DialogSkip : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::ToggleField m_AutoSelectDialog;

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

