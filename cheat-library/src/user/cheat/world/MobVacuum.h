#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>

#include <cheat/game/Entity.h>

#include <il2cpp-appdata.h>

namespace cheat::feature 
{

	class MobVacuum : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::ToggleField m_IncludeAnimals;
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
	
		void OnGameUpdate();
	private:
		MobVacuum();

		bool IsEntityForVac(game::Entity* entity);
		void OnMoveSync(uint32_t entityId, app::MotionInfo* syncInfo);
		app::Vector3 CalcMobVacTargetPos();
	};
}

