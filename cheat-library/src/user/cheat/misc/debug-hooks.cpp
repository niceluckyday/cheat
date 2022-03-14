#include "pch-il2cpp.h"
#include <cheat/cheat.h>

#include <Windows.h>
#include <iostream>
#include <vector>

#include <magic_enum.hpp>

#include <helpers.h>
#include <common/HookManager.h>
#include <gcclib/Logger.h>
#include <common/Config.h>

// Raise when player start game loggin (after press a door)
// Contains information about player pc and game integrity
static void SendInfo_Hook(app::NetworkManager_1* __this, app::GKOJAICIOPA* info, MethodInfo* method) 
{
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

void Lua_xlua_pushasciistring_Hook(void* __this, void* L, app::String* str, MethodInfo* method) 
{
    LOG_DEBUG("Pushed string: %s", il2cppi_to_string(str).c_str());
    callOrigin(Lua_xlua_pushasciistring_Hook, __this, L, str, method);
}

static int checkCount = 0;
void* LuaEnv_DoString_Hook(void* __this, app::Byte__Array* chunk, app::String* chunkName, void* env, MethodInfo* method)
{
    if (checkCount > 0)
    {
        LOG_DEBUG("After size %d; name: %s", chunk->bounds == nullptr ? chunk->max_length : chunk->bounds->length, il2cppi_to_string(chunkName).c_str());
        checkCount--;
    }
    return callOrigin(LuaEnv_DoString_Hook, __this, chunk, chunkName, env, method);
}

void LuaShellManager_DoString_Hook(void* __this, app::Byte__Array* byteArray, MethodInfo* method)
{
    LOG_DEBUG("Size %d", byteArray->bounds == nullptr ? byteArray->max_length : byteArray->bounds->length);
    checkCount = 10;
    callOrigin(LuaShellManager_DoString_Hook, __this, byteArray, method);
}

void LuaShellManager_ReportLuaShellResult_Hook(void* __this, app::String* type, app::String* value, MethodInfo* method)
{
    std::cout << "Type: " << il2cppi_to_string(type) << std::endl;
    std::cout << "Value: " << il2cppi_to_string(value) << std::endl;
    callOrigin(LuaShellManager_ReportLuaShellResult_Hook, __this, type, value, method);
}

void InitDebugHooks() 
{
    // HookManager::install(app::LuaShellManager_ReportLuaShellResult, LuaShellManager_ReportLuaShellResult_Hook);
    // HookManager::install(app::LuaShellManager_DoString, LuaShellManager_DoString_Hook);
    // HookManager::install(app::LuaEnv_DoString, LuaEnv_DoString_Hook);
    // HookManager::install(app::Lua_xlua_pushasciistring, Lua_xlua_pushasciistring_Hook);
    
    // HookManager::install(app::GameLogin_SendInfo_2, SendInfo_Hook);
    // LOG_DEBUG("Hooked GameLogin::SendGameInfo. Origin at 0x%p", HookManager::getOrigin(SendInfo_Hook));
}