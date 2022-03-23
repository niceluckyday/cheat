#pragma once
#include <cheat/Feature.h>
#include <common/config/Config.h>

namespace cheat::feature 
{

	class InfiniteStamina : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::BaseField<bool> m_PacketReplacement;

		static InfiniteStamina& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override;
		void DrawStatus() override;

		virtual bool NeedInfoDraw() const override { return false; };
		void DrawInfo() override {};
	
		bool OnPropertySet(app::PropType__Enum propType);
		void OnMoveSync(uint32_t entityId, app::MotionInfo* syncInfo);

	private:
		InfiniteStamina();
	};
}

