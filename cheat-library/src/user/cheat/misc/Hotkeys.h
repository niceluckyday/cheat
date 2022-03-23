#pragma once
#include <cheat-base/cheat/Feature.h>

namespace cheat::feature 
{

	class Hotkeys : public Feature
    {
	public:
		static Hotkeys& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override { return false; };
		void DrawStatus() override {};

		virtual bool NeedInfoDraw() const override { return false; };
		void DrawInfo() override {};
	
	private:
		Hotkeys();
	};
}

