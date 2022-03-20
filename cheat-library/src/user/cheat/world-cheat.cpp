#include <pch-il2cpp.h>
#include "cheat.h"

#include <vector>
#include <ctime>
#include <chrono>
#include <unordered_set>
#include <random>

#include <magic_enum.hpp>
#include <gcclib/Logger.h>

#include <helpers.h>
#include <il2cpp-appdata.h>
#include <common/HookManager.h>
#include <common/Config.h>
#include <common/thread-safe.h>
#include <common/Patch.h>
#include <common/GlobalEvents.h>

// Raised when monster trying to do skill. Attack also is skill.
// We just block if dumb mob enabled, so mob will not attack player.
static void VCMonsterAIController_TryDoSkill_Hook(void* __this, uint32_t skillID, MethodInfo* method) 
{
    if (Config::cfgDumbEnemiesEnabled.GetValue())
        return;
    callOrigin(VCMonsterAIController_TryDoSkill_Hook, __this, skillID, method);
}

// Raised when dialog view updating
// We call free click, if autotalk enabled, that means we just emulate user click
// When appear dialog choose we create notify with dialog select first item.
static void InLevelCutScenePageContext_UpdateView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method)
{
    callOrigin(InLevelCutScenePageContext_UpdateView_Hook, __this, method);

    if (!Config::cfgAutoTalkEnabled.GetValue())
        return;

    auto talkDialog = __this->fields._talkDialog;
    if (talkDialog == nullptr)
        return;

    if (talkDialog->fields._inSelect)
    {
        int32_t value = 0;
        auto object = il2cpp_value_box((Il2CppClass*)*app::Int32__TypeInfo, &value);
        auto notify = app::Notify_CreateNotify_1(nullptr, app::AJAPIFPNFKP__Enum::DialogSelectItemNotify, (app::Object*)object, nullptr);
        app::TalkDialogContext_OnDialogSelectItem(talkDialog, &notify, nullptr);
    }
    else
        app::InLevelCutScenePageContext_OnFreeClick(__this, method);
}

// Auto loot
static SafeQueue<uint32_t> toBeLootedItems;
static SafeValue<int64_t> nextLootTime { 0 };

static bool AutoLootOnCreateButton(app::BaseEntity* entity) 
{
    if (!Config::cfgAutoLootEnabled)
        return false;

    auto itemModule = GetSingleton(ItemModule);
    if (itemModule == nullptr)
        return false;

    auto entityId = entity->fields._runtimeID_k__BackingField;
    if (Config::cfgAutoLootDelayTime == 0)
    {
        app::ItemModule_PickItem(itemModule, entityId, nullptr);
        return true;
    }

    toBeLootedItems.push(entityId);
    return false;
}

static void AutoLootUpdate()
{
    auto currentTime = GetCurrentTimeMillisec();
    if (currentTime < nextLootTime)
        return;

    auto entityManager = GetSingleton(EntityManager);
    if (entityManager == nullptr)
        return;

    auto entityId = toBeLootedItems.pop();
    if (!entityId)
        return;

    auto itemModule = GetSingleton(ItemModule);
    if (itemModule == nullptr)
        return;

    auto entity = app::EntityManager_GetValidEntity(entityManager, *entityId, nullptr);
    if (entity == nullptr)
        return;

    app::ItemModule_PickItem(itemModule, *entityId, nullptr);
    nextLootTime = currentTime + (int)Config::cfgAutoLootDelayTime;
}

void LCSelectPickup_AddInteeBtnByID_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method)
{
    bool canceled = AutoLootOnCreateButton(entity);
    if (!canceled)
        callOrigin(LCSelectPickup_AddInteeBtnByID_Hook, __this, entity, method);
}

bool LCSelectPickup_IsInPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method) 
{
    if (entity == nullptr)
        return false;

    if (Config::cfgAutoLootEnabled && Config::cfgAutoLootCustomRangeEnabled)
    {
        auto avatarPos = GetAvatarRelativePosition();
        auto entityPos = GetRelativePosition(entity);
        return app::Vector3_Distance(nullptr, avatarPos, entityPos, nullptr) < Config::cfgAutoLootCustomRange;
    }

    return callOrigin(LCSelectPickup_IsInPosition_Hook, __this, entity, method);
}

bool LCSelectPickup_IsOutPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method)
{
    if (entity == nullptr)
        return false;

    if (Config::cfgAutoLootEnabled && Config::cfgAutoLootCustomRangeEnabled)
        return LCSelectPickup_IsInPosition_Hook(__this, entity, method);

    return callOrigin(LCSelectPickup_IsOutPosition_Hook, __this, entity, method);
}

static void KillAuraUpdate()
{
    static std::default_random_engine generator;
    static std::uniform_int_distribution<int> distribution(-50, 50);

    static uint64_t nextAttackTime = 0;
    static std::map<uint32_t, uint64_t> monsterRepeatTimeMap;
    static std::queue<app::BaseEntity*> attackQueue;
    static std::unordered_set<uint32_t> attackSet;

    if (!Config::cfgKillAura)
        return;

    auto eventManager = GetSingleton(EventManager);
    if (eventManager == nullptr || *app::CreateCrashEvent__MethodInfo == nullptr)
        return;

    auto currentTime = GetCurrentTimeMillisec();
    if (currentTime < nextAttackTime)
        return;

    for (const auto& monster : FindEntities(GetMonsterFilter()))
    {
        auto monsterID = monster->fields._runtimeID_k__BackingField;

        if (attackSet.count(monsterID) > 0)
            continue;

        if (monsterRepeatTimeMap.count(monsterID) > 0 && monsterRepeatTimeMap[monsterID] > currentTime)
            continue;

        auto combat = app::BaseEntity_GetBaseCombat(monster, *app::BaseEntity_GetBaseCombat__MethodInfo);
        if (combat == nullptr)
            continue;

        auto combatProp = combat->fields._combatProperty_k__BackingField;
        if (combatProp == nullptr)
            continue;

        auto maxHP = app::SafeFloat_GetValue(nullptr, combatProp->fields.maxHP, nullptr);
        auto isLockHp = app::FixedBoolStack_get_value(combatProp->fields.islockHP, nullptr);
        auto isInvincible = app::FixedBoolStack_get_value(combatProp->fields.isInvincible, nullptr);
        auto HP = app::SafeFloat_GetValue(nullptr, combatProp->fields.HP, nullptr);
        if (maxHP < 10 || HP < 2 || isLockHp || isInvincible)
            continue;

        if (Config::cfgKillAuraOnlyTarget && combat->fields._attackTarget.runtimeID != GetAvatarRuntimeId())
            continue;

        if (GetDistToAvatar(monster) > Config::cfgKillAuraRange)
            continue;

        attackQueue.push(monster);
        attackSet.insert(monsterID);
    }

    if (attackQueue.empty())
        return;

    auto monster = attackQueue.front();
    attackQueue.pop();

    auto monsterID = monster->fields._runtimeID_k__BackingField;
    attackSet.erase(monsterID);

    auto combat = app::BaseEntity_GetBaseCombat(monster, *app::BaseEntity_GetBaseCombat__MethodInfo);
    auto maxHP = app::SafeFloat_GetValue(nullptr, combat->fields._combatProperty_k__BackingField->fields.maxHP, nullptr);

    auto crashEvt = app::CreateCrashEvent(nullptr, *app::CreateCrashEvent__MethodInfo);
    app::EvtCrash_Init(crashEvt, monsterID, nullptr);
    crashEvt->fields.maxHp = maxHP;
    crashEvt->fields.velChange = 1000;
    crashEvt->fields.hitPos = app::BaseEntity_GetAbsolutePosition(monster, nullptr);

    app::EventManager_FireEvent(eventManager, reinterpret_cast<app::BaseEvent*>(crashEvt), false, nullptr);

    monsterRepeatTimeMap[monsterID] = currentTime + (int)Config::cfgKillAuraRepeatDelayTime + distribution(generator);

    nextAttackTime = currentTime + (int)Config::cfgKillAuraAttackDelayTime + distribution(generator);
}


static void OnGameUpdate() 
{
    AutoLootUpdate();
    KillAuraUpdate();
}

void InitWorldCheats()
{
    // Dialog skip
    HookManager::install(app::InLevelCutScenePageContext_UpdateView, InLevelCutScenePageContext_UpdateView_Hook);

    // Dumb enemies
    HookManager::install(app::VCMonsterAIController_TryDoSkill, VCMonsterAIController_TryDoSkill_Hook);

    // Auto loot
    HookManager::install(app::LCSelectPickup_AddInteeBtnByID, LCSelectPickup_AddInteeBtnByID_Hook);
    HookManager::install(app::LCSelectPickup_IsInPosition, LCSelectPickup_IsInPosition_Hook);
    HookManager::install(app::LCSelectPickup_IsOutPosition, LCSelectPickup_IsOutPosition_Hook);

    GlobalEvents::GameUpdateEvent += FREE_METHOD_HANDLER(OnGameUpdate);

    LOG_DEBUG("Initialized");
}

