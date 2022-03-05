#include <pch-il2cpp.h>
#include "cheat.h"

#include <common/Config.h>
#include <common/GlobalEvents.h>

static bool OnKeyUp(short key);

void InitCheats() {

	InitProtectionBypass(); // Removes protection
	InitDebugHooks(); // Hooks for debbug information
	
	InitMapTPHooks(); // Map teleport hooks
	InitPlayerCheatHooks(); // Cheats for player

    GlobalEvents::KeyUpEvent += FREE_METHOD_HANDLER(OnKeyUp);
}

static bool OnKeyUp(short key) 
{
    if (Config::cfgCheatWindowShowed.GetValue())
        return true;

    for (auto& field : Config::GetToggleFields())
    {
        if (field.GetHotkey()->IsPressed(key))
        {
            bool* value = field.GetValuePtr();
            *value = !*value;
            field.Check();
        }
    }

    return true;
}