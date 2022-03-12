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
        ImGui::Text("For current time, only skips talk dialog, but don't skip choose dialog.");
        ConfigWidget(Config::cfgAutoTalkEnabled, "Automatic continue dialog. Don't skip talk momentaly.");
        ConfigWidget(Config::cfgTalkSkipEnabled, "Fast skip dialog.");
        ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "Fast talk skip is unstable. Can produce dialog suspend, especialy in quest dialogs.");
    }
    EndGroupPanel();
}

std::string WorldModule::GetName()
{
    return "World";
}