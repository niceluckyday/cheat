#include "pch-il2cpp.h"
#include "HotkeysModule.h"

#include <imgui.h>

#include <common/Config.h>
#include <gui/gui-util.h>

void HotkeysModule::Draw()
{
    for (auto& field : Config::GetToggleFields()) {
        ConfigWidget(field.GetHotkeyField());
    }
}

std::string HotkeysModule::GetName()
{
    return "Hotkeys";
}