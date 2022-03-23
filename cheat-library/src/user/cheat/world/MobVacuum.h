#pragma once
#include <cheat/Feature.h>
#include <common/config/Config.h>

namespace cheat::feature 
{

	class MobVacuum : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::BaseField<float> m_Speed;
		config::field::BaseField<float> m_Radius;
		config::field::BaseField<float> m_Distance;
		config::field::BaseField<bool> m_OnlyTarget;
		config::field::BaseField<bool> m_Instantly;

		static MobVacuum& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override;
		void DrawStatus() override;

		virtual bool NeedInfoDraw() const override { return false; };
		void DrawInfo() override {};
	
		void OnGameUpdate();
	private:
		MobVacuum();

		bool IsEntityForVac(app::BaseEntity* entity);
		void OnMoveSync(uint32_t entityId, app::MotionInfo* syncInfo);
		app::Vector3 CalcMobVacTargetPos();
	};
}

