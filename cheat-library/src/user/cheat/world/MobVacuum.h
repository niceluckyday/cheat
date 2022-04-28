#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/config.h>

#include <cheat/game/Entity.h>

#include <il2cpp-appdata.h>

namespace cheat::feature 
{

	class MobVacuum : public Feature
    {
	public:
		config::Field<config::ToggleHotkey> f_Enabled;
		config::Field<config::ToggleHotkey> f_IncludeAnimals;
		config::Field<float> f_Speed;
		config::Field<float> f_Radius;
		config::Field<float> f_Distance;
		config::Field<bool> f_OnlyTarget;
		config::Field<bool> f_Instantly;

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

