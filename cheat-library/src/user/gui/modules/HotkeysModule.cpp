#include "pch-il2cpp.h"
#include "HotkeysModule.h"

#include <imgui.h>

#include <gui/gui-util.h>
#include <util/Config.h>

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