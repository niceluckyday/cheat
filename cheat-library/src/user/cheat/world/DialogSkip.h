#pragma once
#include <cheat/Feature.h>
#include <common/config/Config.h>

namespace cheat::feature 
{

	class DialogSkip : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;

		static DialogSkip& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override;
		void DrawStatus() override;

		virtual bool NeedInfoDraw() const override { return false; };
		void DrawInfo() override {};

		void OnCutScenePageUpdate(app::InLevelCutScenePageContext* context);
	
	private:
		DialogSkip();
	};
}

