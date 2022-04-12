#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>
#include <cheat-base/config/field/HotkeyField.h>

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
	
	private:
		Settings();
	};
}

