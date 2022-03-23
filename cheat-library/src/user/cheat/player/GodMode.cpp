#include "pch-il2cpp.h"
#include "GodMode.h"

#include <imgui.h>
#include <common/util.h>
#include <helpers.h>
#include <gui/gui-util.h>
#include <common/HookManager.h>

namespace cheat::feature 
{
    static bool Miscs_CheckTargetAttackable_Hook(void* __this, app::BaseEntity* attacker, app::BaseEntity* target, MethodInfo* method);
    static void VCHumanoidMove_NotifyLandVelocity_Hook(app::VCHumanoidMove* __this, app::Vector3 velocity, float reachMaxDownVelocityTime, MethodInfo* method);

    GodMode::GodMode() : Feature(),
        NFF(m_Enabled, "God mode", "GodModeEnabled", "Player", false)
    {
		// HookManager::install(app::LCBaseCombat_FireBeingHitEvent, LCBaseCombat_FireBeingHitEvent_Hook);
		HookManager::install(app::VCHumanoidMove_NotifyLandVelocity, VCHumanoidMove_NotifyLandVelocity_Hook);
		HookManager::install(app::Miscs_CheckTargetAttackable, Miscs_CheckTargetAttackable_Hook);
    }

    const FeatureGUIInfo& GodMode::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", "Player", false };
        return info;
    }

    void GodMode::DrawMain()
    {
        ConfigWidget(m_Enabled, "Enables god mode. (No income damage); May not work with some types of damage.");
    }

    bool GodMode::NeedStatusDraw() const
{
        return m_Enabled;
    }

    void GodMode::DrawStatus() 
    {
        ImGui::Text("God mode");
    }

    GodMode& GodMode::GetInstance()
    {
        static GodMode instance;
        return instance;
    }

	// Attack immunity (return false when target is avatar, that mean avatar entity isn't attackable)
	static bool Miscs_CheckTargetAttackable_Hook(void* __this, app::BaseEntity* attacker, app::BaseEntity* target, MethodInfo* method)
	{
        auto& gm = GodMode::GetInstance();
		if (gm.m_Enabled && target->fields.entityType == app::EntityType__Enum_1::Avatar)
			return false;

		return callOrigin(Miscs_CheckTargetAttackable_Hook, __this, attacker, target, method);
	}

	// Raised when avatar fall on ground.
    // Sending fall speed, and how many time pass from gain max fall speed (~30m/s).
    // To disable fall damage reset reachMaxDownVelocityTime and decrease fall velocity.
	static void VCHumanoidMove_NotifyLandVelocity_Hook(app::VCHumanoidMove* __this, app::Vector3 velocity, float reachMaxDownVelocityTime, MethodInfo* method)
	{
        auto& gm = GodMode::GetInstance();
		if (gm.m_Enabled && -velocity.y > 13)
		{
			float randAdd = (float)(std::rand() % 1000) / 1000;
			velocity.y = -8 - randAdd;
			reachMaxDownVelocityTime = 0;
		}
		// LOG_DEBUG("%s, %f", il2cppi_to_string(velocity).c_str(), reachMaxDownVelocityTime);
		callOrigin(VCHumanoidMove_NotifyLandVelocity_Hook, __this, velocity, reachMaxDownVelocityTime, method);
	}

    // Analog function for disable attack damage (Thanks to Taiga74164)
    //void LCBaseCombat_FireBeingHitEvent_Hook(app::LCBaseCombat* __this, uint32_t attackeeRuntimeID, app::AttackResult* attackResult, MethodInfo* method) 
    //{
    //    auto avatarEntity = GetAvatarEntity();
    //    if (avatarEntity != nullptr && Config::cfgGodModEnable.GetValue() && avatarEntity->fields._runtimeID_k__BackingField == attackeeRuntimeID)
    //        return;
    //
    //    return callOrigin(LCBaseCombat_FireBeingHitEvent_Hook, __this, attackeeRuntimeID, attackResult, method);
    //}
}

