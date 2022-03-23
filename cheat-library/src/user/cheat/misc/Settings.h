#pragma once
#include <cheat/Feature.h>
#include <common/config/Config.h>

namespace cheat::feature 
{

	class Settings : public Feature
    {
	public:
		config::field::HotkeyField m_MenuKey;

		config::field::BaseField<bool> m_StatusMove;
		config::field::BaseField<bool> m_StatusShow;

		config::field::BaseField<bool> m_InfoMove;
		config::field::BaseField<bool> m_InfoShow;

		config::field::BaseField<bool> m_ConsoleLogging;
		config::field::BaseField<bool> m_FileLogging;

		static Settings& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override { return false; };;
		void DrawStatus() override {};

		virtual bool NeedInfoDraw() const override { return false; };
		void DrawInfo() override {};
	
	private:
		Settings();
	};
}

