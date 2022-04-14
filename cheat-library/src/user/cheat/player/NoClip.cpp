#include "pch-il2cpp.h"
#include "NoClip.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>

namespace cheat::feature 
{
	static void HumanoidMoveFSM_LateTick_Hook(void* __this, float deltaTime, MethodInfo* method);

    NoClip::NoClip() : Feature(),
        NF(m_Enabled,        "No clip",            "NoClip", false),
        NF(m_Speed,          "Speed",              "NoClip", 5.5f),
        NF(m_CameraRelative, "Relative to camera", "NoClip", true),
		NF(m_OverrideSpeed,  "Override speed",     "NoClip", false),
		NF(m_OverrideSpeedValue, "Override speed value", "NoClip", 1.0f)
    {
		HookManager::install(app::HumanoidMoveFSM_LateTick, HumanoidMoveFSM_LateTick_Hook);

		events::GameUpdateEvent += MY_METHOD_HANDLER(NoClip::OnGameUpdate);
		events::MoveSyncEvent += MY_METHOD_HANDLER(NoClip::OnMoveSync);
    }

    const FeatureGUIInfo& NoClip::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "No clip", "Player", true };
        return info;
    }

    void NoClip::DrawMain()
    {
		ConfigWidget("Enabled", m_Enabled, "Enables no clip.\n" \
            "For move use ('W', 'A', 'S', 'D', 'Space', 'Shift')");

		ConfigWidget(m_Speed, 0.1f, 2, 100, "No clip move speed.\n"\
            "It's not recommended to set value above 5.");
		
        ConfigWidget(m_CameraRelative, "Move performing relative to camera direction. Not avatar facing direction.");
		ConfigWidget(m_OverrideSpeed, "Override move speed with value. Pressing LeftCtrl will make you move faster/slower depending on the value you set.");
    }

    bool NoClip::NeedStatusDraw() const
{
        return m_Enabled;
    }

    void NoClip::DrawStatus() 
    {
        ImGui::Text("NoClip [%.01f|%s]", m_Speed.value(), m_CameraRelative ? "CR" : "PR");
    }

    NoClip& NoClip::GetInstance()
    {
        static NoClip instance;
        return instance;
    }

	// No clip update function.
	// We just disabling collision detect and move avatar when no clip moving keys pressed.
	void NoClip::OnGameUpdate()
	{
		static bool isApplied = false;

		auto& manager = game::EntityManager::instance();
		
		if (!m_Enabled && isApplied)
		{
			auto avatarEntity = manager.avatar();
			if (avatarEntity->raw() == nullptr || !app::BaseEntity_IsActive(avatarEntity->raw(), nullptr))
				return;

			auto rigidBody = app::BaseEntity_GetRigidbody(avatarEntity->raw(), nullptr);
			app::Rigidbody_set_detectCollisions(rigidBody, true, nullptr);
			isApplied = false;
		}

		if (!m_Enabled)
			return;

		isApplied = true;

		auto avatarEntity = manager.avatar();
		if (avatarEntity->raw() == nullptr || !app::BaseEntity_IsActive(avatarEntity->raw(), nullptr))
			return;

		auto baseMove = app::BaseEntity_GetMoveComponent_1(avatarEntity->raw(), *app::BaseEntity_GetMoveComponent_1__MethodInfo);
		if (baseMove == nullptr)
			return;

		auto rigidBody = app::BaseEntity_GetRigidbody(avatarEntity->raw(), nullptr);
		app::Rigidbody_set_detectCollisions(rigidBody, false, nullptr);

		auto cameraEntity = (app::BaseEntity*)manager.mainCamera();
		auto relativeEntity = m_CameraRelative ? cameraEntity : avatarEntity->raw();

		float temp_speed = m_Speed.value();
		if (Hotkey(VK_LCONTROL).IsPressed())
			temp_speed = m_OverrideSpeedValue.value(); 

		app::Vector3 dir = {};
		if (Hotkey('W').IsPressed())
			dir = dir + app::BaseEntity_GetForward(relativeEntity, nullptr);

		if (Hotkey('S').IsPressed())
			dir = dir - app::BaseEntity_GetForward(relativeEntity, nullptr);

		if (Hotkey('D').IsPressed())
			dir = dir + app::BaseEntity_GetRight(relativeEntity, nullptr);

		if (Hotkey('A').IsPressed())
			dir = dir - app::BaseEntity_GetRight(relativeEntity, nullptr);

		if (Hotkey(VK_SPACE).IsPressed())
			dir = dir + app::BaseEntity_GetUp(avatarEntity->raw(), nullptr);

		if (Hotkey(ImGuiKey_ModShift).IsPressed())
			dir = dir - app::BaseEntity_GetUp(avatarEntity->raw(), nullptr);

		app::Vector3 prevPos = avatarEntity->relativePosition();
		if (IsVectorZero(prevPos))
			return;

		float deltaTime = app::Time_get_deltaTime(nullptr, nullptr);

		app::Vector3 newPos = prevPos + dir * m_Speed * deltaTime;
		avatarEntity->setRelativePosition(newPos);
	}

	// Fixing player sync packets when no clip
	void NoClip::OnMoveSync(uint32_t entityId, app::MotionInfo* syncInfo)
	{
		static app::Vector3 prevPosition = {};
		static int64_t prevSyncTime = 0;

		if (!m_Enabled)
		{
			prevSyncTime = 0;
			return;
		}

		auto& manager = game::EntityManager::instance();
		if (manager.avatar()->runtimeID() != entityId)
			return;

		auto avatarEntity = manager.avatar();
		if (avatarEntity == nullptr)
			return;

		auto avatarPosition = avatarEntity->absolutePosition();
		auto currentTime = util::GetCurrentTimeMillisec();
		if (prevSyncTime > 0)
		{
			auto posDiff = avatarPosition - prevPosition;
			auto timeDiff = ((float)(currentTime - prevSyncTime)) / 1000;
			auto velocity = posDiff / timeDiff;

			auto speed = GetVectorMagnitude(velocity);
			if (speed > 0.1)
			{
				syncInfo->fields.motionState = (speed < 2) ? app::MotionState__Enum::MotionWalk : app::MotionState__Enum::MotionRun;

				syncInfo->fields.speed_->fields.x = velocity.x;
				syncInfo->fields.speed_->fields.y = velocity.y;
				syncInfo->fields.speed_->fields.z = velocity.z;
			}

			syncInfo->fields.pos_->fields.x = avatarPosition.x;
			syncInfo->fields.pos_->fields.y = avatarPosition.y;
			syncInfo->fields.pos_->fields.z = avatarPosition.z;
		}

		prevPosition = avatarPosition;
		prevSyncTime = currentTime;
	}

	// Disabling standard motion performing.
	// This disabling any animations, climb, jump, swim and so on.
	// But when it disabled, MoveSync sending our last position, so needs to update position in packet.
	static void HumanoidMoveFSM_LateTick_Hook(void* __this, float deltaTime, MethodInfo* method)
	{
		NoClip& noClip = NoClip::GetInstance();
		if (noClip.m_Enabled)
			return;

		callOrigin(HumanoidMoveFSM_LateTick_Hook, __this, deltaTime, method);
	}
}

