#include "pch-il2cpp.h"
#include <cheat/cheat.h>

#include <Windows.h>
#include <iostream>
#include <vector>

#include <magic_enum.hpp>

#include <helpers.h>
#include <common/HookManager.h>
#include <common/Logger.h>
#include <common/Config.h>


static void SendInfo_Hook(app::NetworkManager_1* __this, app::GKOJAICIOPA* info, MethodInfo* method) {
    LOG_TRACE("Game sending game info to server.");
    LOG_TRACE("Content: ");

#define printString(i) if (info->fields.string_ ## i > (void *)1 && info->fields.string_ ## i ##->fields.length > 0)\
    LOG_TRACE("\tfield#%d: %s", i ,il2cppi_to_string(info->fields.string_ ## i).c_str());

    printString(1);
    printString(2);
    printString(3);
    printString(4);
    printString(5);
    printString(6);
    printString(7);
    printString(8);
    printString(9);
    printString(10);
    printString(11);
    printString(12);
    printString(13);
    printString(14);
    printString(15);
    printString(16);

#undef printString

    callOrigin(SendInfo_Hook, __this, info, method);
}

static void BaseActor_SpawnMonster_Hook(app::BaseActor* __this, uint32_t configID, uint32_t level, app::Vector3 bornPos, app::Vector3 bornEuler,
    float scale, uint32_t sceneID, app::String* alias, uint32_t questId, uint32_t roomId, MethodInfo* method) {
    BaseActor_SpawnMonster_Hook(__this, configID, level, bornPos, bornEuler, scale, sceneID, alias, questId, roomId, method);

    LOG_DEBUG("Created monster");
}

static void SetValue(app::SafeFloat* safeFloat, float value) {
    app::SafeFloat_SetValue((app::SafeFloat__Boxed*)((char*)safeFloat - 0x10), value, nullptr);
}


static void SendAttackEventToServer_Hook(app::BKFGGJFIIKC* __this, app::AttackLanded* attackInfo, MethodInfo* method)
{
    auto attackResult = attackInfo->fields._attackResult->fields;
    LOG_DEBUG("Attack event {damage: %f, is_crit: %s}", attackResult.damage, attackResult.critical ? "true" : "false");
    LOG_DEBUG("Attack event {tag: %s}", il2cppi_to_string(attackResult.attackTag).c_str());
    attackInfo->fields._attackResult->fields.damageShield = 0;
    attackInfo->fields._attackResult->fields.damage = 0;

    callOrigin(SendAttackEventToServer_Hook, __this, attackInfo, method);
}


static void VCBaseCombat_DoAttackLanded_Hook(void* __this, app::AttackLanded* attackInfo, MethodInfo* method) 
{
    LOG_DEBUG("DO attack %f", attackInfo->fields._attackResult->fields.damage);
    attackInfo->fields._attackResult->fields.damage = 1000;
    callOrigin(VCBaseCombat_DoAttackLanded_Hook, __this, attackInfo, method);
}

static app::AttackResult* AttackResult_CreateAttackResult_2_Hook(void* __this, app::AttackResult_1* attackProto, MethodInfo* method) 
{
    LOG_DEBUG("Create attack result %f", attackProto->fields.damage_);
    attackProto->fields.damage_ = 1000;
    callOrigin(AttackResult_CreateAttackResult_2_Hook, __this, attackProto, method);
}


void InitDebugHooks() 
{
    //HookManager::install(app::AttackResult_CreateAttackResult_2, AttackResult_CreateAttackResult_2_Hook);
    //HookManager::install(app::SendAttackEventToServer, SendAttackEventToServer_Hook);
    //HookManager::install(app::VCBaseCombat_DoAttackLanded, VCBaseCombat_DoAttackLanded_Hook);

    //HookManager::install(app::BaseActor_SpawnMonster, BaseActor_SpawnMonster_Hook);

    HookManager::install(app::GameLogin_SendInfo_2, SendInfo_Hook);
    LOG_DEBUG("Hooked GameLogin::SendGameInfo. Origin at 0x%p", HookManager::getOrigin(SendInfo_Hook));
}