#include "pch-il2cpp.h"

#include "debug-hooks.h"

#include "util/HookManager.h"
#include "helpers.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include <util/Logger.h>

void __stdcall SendInfo_Hook(app::GameLogin* __this, app::GKOJAICIOPA* info, MethodInfo* method) {
    LOG_TRACE("Game sending game info to server.");
    LOG_TRACE("Content: ");

#define printString(i) if (info->fields.string_ ## i > (void *)1 && info->fields.string_ ## i ##->fields.length > 0)\
    LOG_TRACE("\tfield#%d: %s", i ,il2cppi_to_string(info->fields.string_ ## i));

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
void InitDebugHooks() 
{
    HookManager::install(app::GameLogin_SendInfo_2, SendInfo_Hook);
    LOG_DEBUG("Hooked GameLogin::SendGameInfo. Origin at 0x%p", HookManager::getOrigin(SendInfo_Hook));
}