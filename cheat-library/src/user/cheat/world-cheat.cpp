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

static void OnToggleFieldChange(ToggleConfigField* field)
{
    TogglePatch(field, Config::cfgDumbEnemiesEnabled, 0x4692062, { 0x75 });
}

void InitWorldCheats()
{
    ToggleConfigField::OnChangedEvent += FREE_METHOD_HANDLER(OnToggleFieldChange);
}
