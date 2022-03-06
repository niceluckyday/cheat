#include <pch-il2cpp.h>
#include "cheat.h"

#include <common/Config.h>
#include <common/GlobalEvents.h>

static void OnKeyUp(short key, bool& cancelled);
static void InitToggleFields();

void InitCheats() {

	InitProtectionBypass(); // Removes protection
	InitDebugHooks(); // Hooks for debbug information
	
	InitMapTPHooks(); // Map teleport hooks
	InitPlayerCheats(); // Cheats for player

    InitWorldCheats();

    GlobalEvents::KeyUpEvent += FREE_METHOD_HANDLER(OnKeyUp);

    InitToggleFields();
}

static void InitToggleFields() {
    for (auto& field : Config::GetToggleFields())
    {
        if (field->GetValue())
            ToggleConfigField::OnChangedEvent(field);   
    }
}

static void OnKeyUp(short key, bool& cancelled) 
{
    if (Config::cfgCheatWindowShowed.GetValue())
        return;

    for (auto& field : Config::GetToggleFields())
    {
        if (field->GetHotkey()->IsPressed(key))
        {
            bool* value = field->GetValuePtr();
            *value = !*value;
            field->Check();
        }
    }
}