#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>
#include <map>

#include <il2cpp-appdata.h>

namespace cheat::feature 
{

	class ProtectionBypass : public Feature
    {
	public:
		config::field::BaseField<bool> m_Enabled;

		static ProtectionBypass& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override { return false; };
		void DrawStatus() override {};

		virtual bool NeedInfoDraw() const override { return false; };
		void DrawInfo() override {};
	
		app::Byte__Array* OnRecordUserData(int32_t nType);

		void Init();

	private:
		ProtectionBypass();
		std::map<int32_t, std::string> m_CorrectSignatures;
	};
}

