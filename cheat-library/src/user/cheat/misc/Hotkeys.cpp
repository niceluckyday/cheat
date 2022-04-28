#include "pch-il2cpp.h"
#include "Hotkeys.h"

#include <helpers.h>

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
        for (auto& field : config::GetFields<config::ToggleHotkey>())
        {
            ConfigWidget(field, nullptr, true);
        }
    }

    Hotkeys& Hotkeys::GetInstance()
    {
        static Hotkeys instance;
        return instance;
    }
}

