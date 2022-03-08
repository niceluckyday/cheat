#include <pch-il2cpp.h>
#include "cheat.h"

#include <vector>

#include <magic_enum.hpp>

#include <helpers.h>
#include <il2cpp-appdata.h>
#include <common/HookManager.h>
#include <common/Config.h>
#include <common/Logger.h>
#include <common/Patch.h>

// Godmode (return false when target is avatar, that mean avatar entity isn't attackable)
static bool Miscs_CheckTargetAttackable_Hook(void* __this, app::BaseEntity* attacker, app::BaseEntity* target, MethodInfo* method)
{
    if (Config::cfgGodModEnable.GetValue() && target->fields.entityType == app::EntityType__Enum_1::Avatar)
        return false;
    return callOrigin(Miscs_CheckTargetAttackable_Hook, __this, attacker, target, method);
}

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
        return;

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

void InitPlayerCheats() 
{
    HookManager::install(app::NetworkManager_1_RequestSceneEntityMoveReq, NetworkManager_1_RequestSceneEntityMoveReq_Hook);
    LOG_TRACE("Hooked NetworkManager_1_RequestSceneEntityMoveReq. Origin at 0x%p", HookManager::getOrigin(NetworkManager_1_RequestSceneEntityMoveReq_Hook));

    // Teleport to mark hooks
    HookManager::install(app::Miscs_CheckTargetAttackable, Miscs_CheckTargetAttackable_Hook);
    LOG_TRACE("Hooked Miscs_CheckTargetAttackable. Origin at 0x%p", HookManager::getOrigin(Miscs_CheckTargetAttackable_Hook));

    HookManager::install(app::AvatarPropDictionary_SetItem, AvatarPropDictionary_SetItem_Hook);
    LOG_TRACE("Hooked AvatarPropDictionary_SetItem. Origin at 0x%p", HookManager::getOrigin(AvatarPropDictionary_SetItem_Hook));

    LOG_DEBUG("Hooks installed");
}
