#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>
#include <cheat-base/thread-safe.h>

namespace cheat::feature 
{

	class AutoLoot : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::BaseField<int> m_DelayTime;
		config::field::BaseField<bool> m_UseCustomRange;
		config::field::BaseField<float> m_CustomRange;

		static AutoLoot& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override;
		void DrawStatus() override;

		virtual bool NeedInfoDraw() const override { return false; };
		void DrawInfo() override {};

		bool OnCreateButton(app::BaseEntity* entity);
		void OnGameUpdate();
		void OnCheckIsInPosition(bool& result, app::BaseEntity* entity);
	
	private:
		
		SafeQueue<uint32_t> toBeLootedItems;
		SafeValue<int64_t> nextLootTime;
		AutoLoot();
	};
}

