#include <pch-il2cpp.h>
#include "cheat.h"

#include <vector>
#include <math.h>

#include <magic_enum.hpp>

#include <helpers.h>
#include <il2cpp-appdata.h>
#include <common/HookManager.h>
#include <common/Config.h>
#include <gcclib/Logger.h>
#include <common/Patch.h>
#include <common/GlobalEvents.h>

// Godmode (return false when target is avatar, that mean avatar entity isn't attackable)
static bool Miscs_CheckTargetAttackable_Hook(void* __this, app::BaseEntity* attacker, app::BaseEntity* target, MethodInfo* method)
{
    if (Config::cfgGodModEnable.GetValue() && target->fields.entityType == app::EntityType__Enum_1::Avatar)
        return false;

    return callOrigin(Miscs_CheckTargetAttackable_Hook, __this, attacker, target, method);
}

// Raised when avatar fall on ground.
// Sending fall speed, and how many time pass from gain max fall speed (~30m/s).
// To disable fall damage reset reachMaxDownVelocityTime and decrease fall velocity.
static void VCHumanoidMove_NotifyLandVelocity_Hook(app::VCHumanoidMove* __this, app::Vector3 velocity, float reachMaxDownVelocityTime, MethodInfo* method) 
{
    if (Config::cfgGodModEnable.GetValue() && -velocity.y > 13) 
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

// Infinite stamina offline mode. Blocks changes for stamina property. 
// Note. Changes received from the server (not sure about this for current time), 
//       that means that server know our stamina, and changes it in client can be detected.
// Not working for water because server sending drown action when your stamina down to zero. (Also guess for now)
static void AvatarPropDictionary_SetItem_Hook(app::Dictionary_2_JNHGGGCKJNA_JKNLDEEBGLL_* __this, app::JNHGGGCKJNA key, app::JKNLDEEBGLL value, MethodInfo* method)
{
    app::PropType__Enum propType = app::AvatarProp_DecodePropType(nullptr, key, nullptr);
    if (Config::cfgInfiniteStaminaEnable.GetValue() && 
        !Config::cfgISMovePacketMode.GetValue() &&
        (propType == app::PropType__Enum::PROP_MAX_STAMINA ||
            propType == app::PropType__Enum::PROP_CUR_PERSIST_STAMINA ||
            propType == app::PropType__Enum::PROP_CUR_TEMPORARY_STAMINA))
        return;

    callOrigin(AvatarPropDictionary_SetItem_Hook, __this, key, value, method);
}

// Infinite stamina packet mode.
// Note. Blocking packets with movement information, to prevent ability server to know stamina info.
//       But server may see incorrect movements. What mode safer don't tested.
static void NetworkManager_1_RequestSceneEntityMoveReq_Hook(app::BKFGGJFIIKC* __this, uint32_t entityId, app::MotionInfo* syncInfo, bool isReliable, uint32_t relseq, MethodInfo* method)
{
    static bool afterDash = false;

    if (!IsSingletonLoaded(EntityManager)) 
    {
        callOrigin(NetworkManager_1_RequestSceneEntityMoveReq_Hook, __this, entityId, syncInfo, isReliable, relseq, method);
        return;
    }
        
    auto entityManager = GetSingleton(EntityManager);
    auto avatarEntity = app::EntityManager_GetCurrentAvatar(entityManager, nullptr);
    if (entityId != avatarEntity->fields._runtimeID_k__BackingField)
    {
        callOrigin(NetworkManager_1_RequestSceneEntityMoveReq_Hook, __this, entityId, syncInfo, isReliable, relseq, method);
        return;
    }

    // LOG_DEBUG("Movement packet: %s", magic_enum::enum_name(syncInfo->fields.motionState).data());
    if (Config::cfgInfiniteStaminaEnable.GetValue() && Config::cfgISMovePacketMode.GetValue())
    {
        auto state = syncInfo->fields.motionState;
        switch (state)
        {
        case app::MotionState__Enum::MotionDash:
        case app::MotionState__Enum::MotionClimb:
        case app::MotionState__Enum::MotionClimbJump:
        case app::MotionState__Enum::MotionStandbyToClimb:
        case app::MotionState__Enum::MotionSwimDash:
        case app::MotionState__Enum::MotionSwimIdle:
        case app::MotionState__Enum::MotionSwimMove:
        case app::MotionState__Enum::MotionSwimJump:
        case app::MotionState__Enum::MotionFly:
        case app::MotionState__Enum::MotionFight:
        case app::MotionState__Enum::MotionDashBeforeShake:
        case app::MotionState__Enum::MotionDangerDash:
            syncInfo->fields.motionState = app::MotionState__Enum::MotionRun;
            break;
        case app::MotionState__Enum::MotionJump:
            if (afterDash)
                syncInfo->fields.motionState = app::MotionState__Enum::MotionRun;
            break;
        }
        if (state != app::MotionState__Enum::MotionJump && state != app::MotionState__Enum::MotionFallOnGround)
            afterDash = state == app::MotionState__Enum::MotionDash;
    }

    callOrigin(NetworkManager_1_RequestSceneEntityMoveReq_Hook, __this, entityId, syncInfo, isReliable, relseq, method);
}

// Check sprint cooldown, we just return true if sprint no cd enabled.
bool HumanoidMoveFSM_CheckSprintCooldown_Hook(void* __this, MethodInfo* method) 
{
    if (Config::cfgNoSprintCDEnable)
        return true;

    return callOrigin(HumanoidMoveFSM_CheckSprintCooldown_Hook, __this, method);
}

bool LCAvatarCombat_IsEnergyMax_Hook(void* __this, MethodInfo* method)
{
    if (Config::cfgNoSkillCDEnable)
        return true;

    return callOrigin(LCAvatarCombat_IsEnergyMax_Hook, __this, method);
}

bool LCAvatarCombat_IsSkillInCD_1_Hook(void* __this, void* skillInfo, MethodInfo* method)
{
    if (Config::cfgNoSkillCDEnable)
        return false;

    return callOrigin(LCAvatarCombat_IsSkillInCD_1_Hook, __this, skillInfo, method);
}

// This function raise when abilities, whose has charge, is charging, like a bow.
// value - increase value
// min and max - bounds of charge.
// So, to charge make full charge instantly, just replace value to maxValue.
void ActorAbilityPlugin_AddDynamicFloatWithRange_Hook(void* __this, app::String* key, float value, float minValue, float maxValue, 
    bool forceDoAtRemote, MethodInfo* method)
{
    if (Config::cfgInstantBowEnable)
        value = maxValue;
    callOrigin(ActorAbilityPlugin_AddDynamicFloatWithRange_Hook, __this, key, value, minValue, maxValue, forceDoAtRemote, method);
}

int CalcCountToKill(float attackDamage, uint32_t targetID) 
{
    auto targetEntity = GetEntityByRuntimeId(targetID);
    if (targetEntity == nullptr)
        return Config::cfgRapidFireMultiplier;

    auto baseCombat = app::BaseEntity_GetBaseCombat(targetEntity, *app::BaseEntity_GetBaseCombat__MethodInfo);
    if (baseCombat == nullptr)
        return Config::cfgRapidFireMultiplier;

    auto safeHP = baseCombat->fields._combatProperty_k__BackingField->fields.HP;
    auto HP = app::SafeFloat_GetValue(nullptr, safeHP, nullptr);
    LOG_DEBUG("HP: %f, attack damage: %f", HP, attackDamage);
    int attackCount = (int)ceil(HP / attackDamage);
    return min(attackCount, 1000);
}

// Raises when any entity do hit event.
// Just recall attack few times (regulating by config)
// It's not tested well, so, I think, anticheat can detect it.
// When new information will be received, I update this comment.
void LCBaseCombat_DoHitEntity_Hook(app::LCBaseCombat* __this, uint32_t targetID, app::AttackResult* attackResult,
    bool ignoreCheckCanBeHitInMP, MethodInfo* method)
{
    if (__this->fields._._.entityRuntimeID != GetAvatarEntity()->fields._runtimeID_k__BackingField || !Config::cfgRapidFire)
        return callOrigin(LCBaseCombat_DoHitEntity_Hook, __this, targetID, attackResult, ignoreCheckCanBeHitInMP, method);

    int countOfAttacks = Config::cfgRapidFireMultiplier.GetValue();
    if (Config::cfgRapidFireOnePunch)
    {
        auto targetEntity = GetEntityByRuntimeId(targetID);
        auto baseCombat = app::BaseEntity_GetBaseCombat(targetEntity, *app::BaseEntity_GetBaseCombat__MethodInfo);
        app::Formula_CalcAttackResult(targetEntity, __this->fields._combatProperty_k__BackingField, 
            baseCombat->fields._combatProperty_k__BackingField, 
            attackResult, GetAvatarEntity(), targetEntity, nullptr);
        countOfAttacks = CalcCountToKill(attackResult->fields.damage - attackResult->fields.damageShield, targetID);
    }
    
    for (int i = 0; i < countOfAttacks; i++)
        callOrigin(LCBaseCombat_DoHitEntity_Hook, __this, targetID, attackResult, ignoreCheckCanBeHitInMP, method);
}

void InitPlayerCheats() 
{
    // God mode
    // HookManager::install(app::LCBaseCombat_FireBeingHitEvent, LCBaseCombat_FireBeingHitEvent_Hook);
    HookManager::install(app::VCHumanoidMove_NotifyLandVelocity, VCHumanoidMove_NotifyLandVelocity_Hook);
    HookManager::install(app::Miscs_CheckTargetAttackable, Miscs_CheckTargetAttackable_Hook);

    // Infinite stamina
    HookManager::install(app::AvatarPropDictionary_SetItem, AvatarPropDictionary_SetItem_Hook);
    HookManager::install(app::NetworkManager_1_RequestSceneEntityMoveReq, NetworkManager_1_RequestSceneEntityMoveReq_Hook);

    // No cooldown
    HookManager::install(app::HumanoidMoveFSM_CheckSprintCooldown, HumanoidMoveFSM_CheckSprintCooldown_Hook);
    HookManager::install(app::LCAvatarCombat_IsEnergyMax, LCAvatarCombat_IsEnergyMax_Hook);
    HookManager::install(app::LCAvatarCombat_IsSkillInCD_1, LCAvatarCombat_IsSkillInCD_1_Hook);
    HookManager::install(app::ActorAbilityPlugin_AddDynamicFloatWithRange, ActorAbilityPlugin_AddDynamicFloatWithRange_Hook);

    // Rapid fire
    HookManager::install(app::LCBaseCombat_DoHitEntity, LCBaseCombat_DoHitEntity_Hook);

    LOG_DEBUG("Initialized");
}
