#include <pch-il2cpp.h>
#include "cheat.h"

#include <common/Config.h>
#include <common/Event.h>
#include <common/GlobalEvents.h>
#include <common/HookManager.h>

static void OnKeyUp(short key, bool& cancelled);
static void InitToggleFields();

static void GameManager_Update_Hook(app::GameManager* __this, MethodInfo* method)
{
    GlobalEvents::GameUpdateEvent();
    callOrigin(GameManager_Update_Hook, __this, method);
}

void InitCheats() 
{
    InitProtectionBypass(); // Removes protection

    // Game thread
    HookManager::install(app::GameManager_Update, GameManager_Update_Hook);

	InitDebugHooks(); // Hooks for debbug information
    InitPacketHooks();

	InitMapTPHooks(); // Map teleport hooks
	InitPlayerCheats(); // Cheats for player
    InitWorldCheats();

    GlobalEvents::KeyUpEvent += FREE_METHOD_HANDLER(OnKeyUp);

    InitToggleFields();
}

static void InitToggleFields() 
{
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
