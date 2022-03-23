#include "pch-il2cpp.h"
#include "Hotkeys.h"

#include <imgui.h>
#include <common/util.h>
#include <common/config/Config.h>
#include <helpers.h>
#include <gui/gui-util.h>

namespace cheat::feature 
{
    Hotkeys::Hotkeys() : Feature() { }

    const FeatureGUIInfo& Hotkeys::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", "Hotkeys", false };
        return info;
    }

    void Hotkeys::DrawMain()
    {
        for (auto& field : config::GetToggleFields())
        {
            auto& hotkeyField = field->GetHotkeyField();
            ConfigWidget(hotkeyField);
        }
    }

    Hotkeys& Hotkeys::GetInstance()
    {
        static Hotkeys instance;
        return instance;
    }
}

