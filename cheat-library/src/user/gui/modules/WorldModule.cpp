#include "pch-il2cpp.h"
#include "WorldModule.h"

#include <imgui.h>

#include <common/Config.h>
#include <gui/gui-util.h>

void WorldModule::Draw()
{
    ConfigWidget(Config::cfgDumbEnemiesEnabled, "Enemies don't respond to player.");
}

std::string WorldModule::GetName()
{
    return "World";
}