#pragma once

#include <il2cpp-appdata.h>

#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>

namespace cheat::feature 
{

	class MapTeleport : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::BaseField<bool> m_DetectHeight;
		config::field::BaseField<float> m_DefaultHeight;
		config::field::HotkeyField m_Key;

		static MapTeleport& GetInstance();

		void TeleportTo(app::Vector3 position, bool needHeightCalc = false, uint32_t sceneId = 0);

		void TeleportTo(app::Vector2 mapPosition);

		void OnGetTargetPos(app::Vector3& position);

		void OnCheckTeleportDistance(bool needTransByServer);

		void OnPerformPlayerTransmit(app::Vector3& position);

		void OnSetAvatarPosition(app::Vector3& position);

		void OnGameUpdate();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override { return false; };
		void DrawStatus() override {};

		virtual bool NeedInfoDraw() const override { return false; };
		void DrawInfo() override {};
		
	private:
		struct TeleportTaskInfo
		{
			bool waitingThread = false;
			bool needHeightCalculation = false;
			int currentStage = 0;
			app::Vector3 targetPosition = {};
			uint32_t sceneId = 0;
			uint32_t waypointId = 0;
		};
		TeleportTaskInfo taskInfo;

		MapTeleport();
	};
}

