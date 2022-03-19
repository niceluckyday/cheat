#include "pch-il2cpp.h"
#include "WorldModule.h"

#include <imgui.h>

#include <common/Config.h>
#include <gui/gui-util.h>

void WorldModule::Draw()
{
    ConfigWidget(Config::cfgDumbEnemiesEnabled, "Enemies don't respond to player.");

    BeginGroupPanel("Dialog", ImVec2(-1.0f, 0.0f));
    {
        //ImGui::Text("For current time, only skips talk dialog, but don't skip choose dialog.");
        ConfigWidget(Config::cfgAutoTalkEnabled, "Automatic continue dialog. Don't skip talk momentaly.");
        // ConfigWidget(Config::cfgTalkSkipEnabled, "Fast skip dialog.");
        // ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "Fast talk skip is unstable. Can produce dialog suspend, especialy in quest dialogs.");
    }
    EndGroupPanel();

    BeginGroupPanel("Auto loot", ImVec2(-1.0f, 0.0f));
    {
        ConfigWidget("Enabled ## AutoLoot", Config::cfgAutoLootEnabled, "Automatic loot dropped items.\n\
            Note: Use custom range or decreasing delay time can be unsafe.");
        ConfigWidget(Config::cfgAutoLootDelayTime, 1, 0, 1000, "Delay (in milleseconds) beetwen looting items.\n\
            Value under 200ms can be unsafe.");
        ConfigWidget(Config::cfgAutoLootCustomRangeEnabled, "Enabling custom looting range.\n\
            Use this feature not recommended, it can be easily detected by server.");
        ConfigWidget(Config::cfgAutoLootCustomRange, 0.1, 0.5, 60, "Custom looting range. It's applying only if use custom range enabled.");
    }
    EndGroupPanel();
}

std::string WorldModule::GetName()
{
    return "World";
}