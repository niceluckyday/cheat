#include "pch-il2cpp.h"

#include "debug-hooks.h"

#include "util/HookManager.h"
#include "helpers.h"
#include <magic_enum.hpp>

#include <iostream>
#include <vector>


#include <Windows.h>
#include <util/Logger.h>

void __stdcall SendInfo_Hook(app::GameLogin* __this, app::GKOJAICIOPA* info, MethodInfo* method) {
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

void BaseActor_SpawnMonster_Hook(app::BaseActor* __this, uint32_t configID, uint32_t level, app::Vector3 bornPos, app::Vector3 bornEuler, 
    float scale, uint32_t sceneID, app::String* alias, uint32_t questId, uint32_t roomId, MethodInfo* method) {
    BaseActor_SpawnMonster_Hook(__this, configID, level, bornPos, bornEuler, scale, sceneID, alias, questId, roomId, method);

    LOG_DEBUG("Created monster");
}

void InitDebugHooks() 
{
    HookManager::install(app::BaseActor_SpawnMonster, BaseActor_SpawnMonster_Hook);

    HookManager::install(app::GameLogin_SendInfo_2, SendInfo_Hook);
    LOG_DEBUG("Hooked GameLogin::SendGameInfo. Origin at 0x%p", HookManager::getOrigin(SendInfo_Hook));
}