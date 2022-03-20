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
static void InfiniteStaminaOnSync(uint32_t entityId, app::MotionInfo* syncInfo)
{
    static bool afterDash = false;

    if (GetAvatarRuntimeId() != entityId)
        return;

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
}

// Check if entity valid for mob vaccum.
static bool IsEntityForVac(app::BaseEntity* entity)
{
    if (!IsEntityFilterValid(entity, GetMonsterFilter()))
        return false;

    if (Config::cfgMobVaccumOnlyTarget)
    {
        auto monsterCombat = app::BaseEntity_GetBaseCombat(entity, *app::BaseEntity_GetBaseCombat__MethodInfo);
        if (monsterCombat == nullptr || monsterCombat->fields._attackTarget.runtimeID != GetAvatarRuntimeId())
            return false;
    }

    auto distance = GetDistToAvatar(entity);
    if (distance > Config::cfgMobVaccumRadius)
        return false;

    return true;
}

// Calculate mob vacum target position.
static app::Vector3 CalcMobVacTargetPos() 
{
    auto avatarEntity = GetAvatarEntity();
    if (avatarEntity == nullptr)
        return {};

    auto avatarRelPos = GetRelativePosition(avatarEntity);
    auto avatarForward = app::BaseEntity_GetForward(avatarEntity, nullptr);
    return avatarRelPos + avatarForward * Config::cfgMobVaccumDistance;
}

// Mob vaccum update function.
// Changes position of monster, if mob vaccum enabled.
static void UpdateMobVaccum()
{
    static auto positions = new std::map<uint32_t, app::Vector3>();

    if (!Config::cfgMobVaccumEnable)
        return;

    app::Vector3 targetPos = CalcMobVacTargetPos();
    if (IsVectorZero(targetPos))
        return;

    auto newPositions = new std::map<uint32_t, app::Vector3>();
    for (const auto& monster : FindEntities(IsEntityForVac))
    {
        if (Config::cfgMobVaccumInstantly)
        {
            SetRelativePosition(monster, targetPos);
            continue;
        }

        uint32_t monsterId = monster->fields._runtimeID_k__BackingField;
        app::Vector3 monsterRelPos = positions->count(monsterId) ? (*positions)[monsterId] : GetRelativePosition(monster);
        app::Vector3 newPosition = {};
        if (app::Vector3_Distance(nullptr, monsterRelPos, targetPos, nullptr) < 0.1)
        {
            newPosition = targetPos;
        }
        else 
        {
            app::Vector3 dir = GetVectorDirection(monsterRelPos, targetPos);
            float deltaTime = app::Time_get_deltaTime(nullptr, nullptr);
            float speed = Config::cfgMobVaccumSpeed;

            newPosition = monsterRelPos + dir * Config::cfgMobVaccumSpeed * deltaTime;
        }

        (*newPositions)[monsterId] = newPosition;
        SetRelativePosition(monster, newPosition);
    }

    delete positions;
    positions = newPositions;
}

// Mob vaccum sync packet replace.
// Replacing move sync speed and motion state.
//   Callow: I think it is more safe method, 
//           because for server monster don't change position instantly.
static void MobVaccumOnSync(uint32_t entityId, app::MotionInfo* syncInfo)
{
    if (!Config::cfgMobVaccumEnable || Config::cfgMobVaccumInstantly)
        return;

    auto entityManager = GetSingleton(EntityManager);
    if (entityManager == nullptr)
        return;

    auto entity = app::EntityManager_GetValidEntity(entityManager, entityId, nullptr);
    if (entity == nullptr)
        return;

    if (!IsEntityForVac(entity))
        return;

    app::Vector3 targetPos = CalcMobVacTargetPos();
    app::Vector3 entityPos = GetRelativePosition(entity);
    if (app::Vector3_Distance(nullptr, targetPos, entityPos, nullptr) < 0.2)
        return;

    float speed = Config::cfgMobVaccumSpeed;
    app::Vector3 dir = GetVectorDirection(targetPos, entityPos);
    app::Vector3 scaledDir = dir * speed;

    syncInfo->fields.speed_->fields.x = scaledDir.x;
    syncInfo->fields.speed_->fields.y = scaledDir.y;
    syncInfo->fields.speed_->fields.z = scaledDir.z;

    switch (syncInfo->fields.motionState)
    {
    case app::MotionState__Enum::MotionStandby:
    case app::MotionState__Enum::MotionStandbyMove:
    case app::MotionState__Enum::MotionWalk:
    case app::MotionState__Enum::MotionDangerDash:
        syncInfo->fields.motionState = app::MotionState__Enum::MotionRun;
    }
}

// Check sprint cooldown, we just return true if sprint no cd enabled.
static bool HumanoidMoveFSM_CheckSprintCooldown_Hook(void* __this, MethodInfo* method) 
{
    if (Config::cfgNoSprintCDEnable)
        return true;

    return callOrigin(HumanoidMoveFSM_CheckSprintCooldown_Hook, __this, method);
}

static bool LCAvatarCombat_IsEnergyMax_Hook(void* __this, MethodInfo* method)
{
    if (Config::cfgNoSkillCDEnable)
        return true;

    return callOrigin(LCAvatarCombat_IsEnergyMax_Hook, __this, method);
}

static bool LCAvatarCombat_IsSkillInCD_1_Hook(void* __this, void* skillInfo, MethodInfo* method)
{
    if (Config::cfgNoSkillCDEnable)
        return false;

    return callOrigin(LCAvatarCombat_IsSkillInCD_1_Hook, __this, skillInfo, method);
}

// This function raise when abilities, whose has charge, is charging, like a bow.
// value - increase value
// min and max - bounds of charge.
// So, to charge make full charge instantly, just replace value to maxValue.
static void ActorAbilityPlugin_AddDynamicFloatWithRange_Hook(void* __this, app::String* key, float value, float minValue, float maxValue, 
    bool forceDoAtRemote, MethodInfo* method)
{
    if (Config::cfgInstantBowEnable)
        value = maxValue;
    callOrigin(ActorAbilityPlugin_AddDynamicFloatWithRange_Hook, __this, key, value, minValue, maxValue, forceDoAtRemote, method);
}

static int CalcCountToKill(float attackDamage, uint32_t targetID) 
{
    if (attackDamage == 0)
        return Config::cfgRapidFireMultiplier;

    auto targetEntity = GetEntityByRuntimeId(targetID);
    if (targetEntity == nullptr)
        return Config::cfgRapidFireMultiplier;

    auto baseCombat = app::BaseEntity_GetBaseCombat(targetEntity, *app::BaseEntity_GetBaseCombat__MethodInfo);
    if (baseCombat == nullptr)
        return Config::cfgRapidFireMultiplier;

    auto safeHP = baseCombat->fields._combatProperty_k__BackingField->fields.HP;
    auto HP = app::SafeFloat_GetValue(nullptr, safeHP, nullptr);
    int attackCount = (int)ceil(HP / attackDamage);
    return std::clamp(attackCount, 1, 1000);
}

// Raises when any entity do hit event.
// Just recall attack few times (regulating by combatProp)
// It's not tested well, so, I think, anticheat can detect it.
// When new information will be received, I update this comment.
static void LCBaseCombat_DoHitEntity_Hook(app::LCBaseCombat* __this, uint32_t targetID, app::AttackResult* attackResult,
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
        countOfAttacks = CalcCountToKill(attackResult->fields.damage, targetID);
    }
    
    for (int i = 0; i < countOfAttacks; i++)
        callOrigin(LCBaseCombat_DoHitEntity_Hook, __this, targetID, attackResult, ignoreCheckCanBeHitInMP, method);
}

// No clip update function.
// We just disabling collision detect and move avatar when noclip moving keys pressed.
static void NoClipUpdate()
{
    static bool enabled = false;

    if (!Config::cfgNoClipEnable && enabled)
    {
        auto avatarEntity = GetAvatarEntity();
        if (avatarEntity == nullptr || !app::BaseEntity_IsActive(avatarEntity, nullptr))
            return;

        auto rigidBody = app::BaseEntity_GetRigidbody(avatarEntity, nullptr);
        app::Rigidbody_set_detectCollisions(rigidBody, true, nullptr);
        enabled = false;
    }

    if (!Config::cfgNoClipEnable)
        return;

    enabled = true;

    auto avatarEntity = GetAvatarEntity();
    if (avatarEntity == nullptr || !app::BaseEntity_IsActive(avatarEntity, nullptr))
        return;

    auto baseMove = app::BaseEntity_GetMoveComponent_1(avatarEntity, *app::BaseEntity_GetMoveComponent_1__MethodInfo);
    if (baseMove == nullptr)
        return;

    auto rigidBody = app::BaseEntity_GetRigidbody(avatarEntity, nullptr);
    app::Rigidbody_set_detectCollisions(rigidBody, false, nullptr);

    auto cameraEntity = (app::BaseEntity*) GetMainCameraEntity();
    auto entity = Config::cfgNoClipCameraMove ? cameraEntity : avatarEntity;

    app::Vector3 dir = {};
    if (Hotkey('W', 0).IsPressed())
        dir = dir + app::BaseEntity_GetForward(entity, nullptr);

    if (Hotkey('S', 0).IsPressed())
        dir = dir - app::BaseEntity_GetForward(entity, nullptr);

    if (Hotkey('D', 0).IsPressed())
        dir = dir + app::BaseEntity_GetRight(entity, nullptr);

    if (Hotkey('A', 0).IsPressed())
        dir = dir - app::BaseEntity_GetRight(entity, nullptr);

    if (Hotkey(VK_SPACE, 0).IsPressed())
        dir = dir + app::BaseEntity_GetUp(avatarEntity, nullptr);

    if (Hotkey(VK_SHIFT, 0).IsPressed())
        dir = dir - app::BaseEntity_GetUp(avatarEntity, nullptr);

    app::Vector3 prevPos = GetRelativePosition(avatarEntity);
    if (IsVectorZero(prevPos))
        return;

    float deltaTime = app::Time_get_deltaTime(nullptr, nullptr);
    float speed = Config::cfgNoClipSpeed;

    app::Vector3 newPos = prevPos + dir * speed * deltaTime;
    SetRelativePosition(avatarEntity, newPos);
}

// Disabling standard motion performing.
// This disabling any animations, climb, jump, swim and so on.
// But when it disabled, MoveSync sending our last position, so needs to update position in packet.
static void HumanoidMoveFSM_LateTick_Hook(void* __this, float deltaTime, MethodInfo* method) 
{
    if (Config::cfgNoClipEnable)
        return;
    
    callOrigin(HumanoidMoveFSM_LateTick_Hook, __this, deltaTime, method);
}

// Fixing player sync packets when noclip
static void NoClipOnSync(uint32_t entityId, app::MotionInfo* syncInfo) 
{
    static app::Vector3 prevPosition = {};
    static int64_t prevSyncTime = 0;

    if (!Config::cfgNoClipEnable)
    {
        prevSyncTime = 0;
        return;
    }
        
    if (GetAvatarRuntimeId() != entityId)
        return;

    auto avatarEntity = GetAvatarEntity();
    if (avatarEntity == nullptr)
        return;

    auto avatarPosition = app::BaseEntity_GetAbsolutePosition(avatarEntity, nullptr);
    auto currentTime = GetCurrentTimeMillisec();

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

static void LevelSyncCombatPlugin_RequestSceneEntityMoveReq_Hook(app::BKFGGJFIIKC* __this, uint32_t entityId, app::MotionInfo* syncInfo,
    bool isReliable, uint32_t relseq, MethodInfo* method)
{
    InfiniteStaminaOnSync(entityId, syncInfo);
    MobVaccumOnSync(entityId, syncInfo);
    NoClipOnSync(entityId, syncInfo);

    callOrigin(LevelSyncCombatPlugin_RequestSceneEntityMoveReq_Hook, __this, entityId, syncInfo, isReliable, relseq, method);
}

static void OnGameUpdate()
{
    UpdateMobVaccum();
    NoClipUpdate();
}

void InitPlayerCheats() 
{
    // God mode
    // HookManager::install(app::LCBaseCombat_FireBeingHitEvent, LCBaseCombat_FireBeingHitEvent_Hook);
    HookManager::install(app::VCHumanoidMove_NotifyLandVelocity, VCHumanoidMove_NotifyLandVelocity_Hook);
    HookManager::install(app::Miscs_CheckTargetAttackable, Miscs_CheckTargetAttackable_Hook);

    // Infinite stamina
    HookManager::install(app::AvatarPropDictionary_SetItem, AvatarPropDictionary_SetItem_Hook);
    HookManager::install(app::LevelSyncCombatPlugin_RequestSceneEntityMoveReq, LevelSyncCombatPlugin_RequestSceneEntityMoveReq_Hook);

    // No cooldown
    HookManager::install(app::HumanoidMoveFSM_CheckSprintCooldown, HumanoidMoveFSM_CheckSprintCooldown_Hook);
    HookManager::install(app::LCAvatarCombat_IsEnergyMax, LCAvatarCombat_IsEnergyMax_Hook);
    HookManager::install(app::LCAvatarCombat_IsSkillInCD_1, LCAvatarCombat_IsSkillInCD_1_Hook);
    HookManager::install(app::ActorAbilityPlugin_AddDynamicFloatWithRange, ActorAbilityPlugin_AddDynamicFloatWithRange_Hook);

    // Rapid fire
    HookManager::install(app::LCBaseCombat_DoHitEntity, LCBaseCombat_DoHitEntity_Hook);

    // No clip
    HookManager::install(app::HumanoidMoveFSM_LateTick, HumanoidMoveFSM_LateTick_Hook);

    GlobalEvents::GameUpdateEvent += FREE_METHOD_HANDLER(OnGameUpdate);

    LOG_DEBUG("Initialized");
}
