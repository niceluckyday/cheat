#include "pch-il2cpp.h"
#include "PlayerModule.h"

#include <imgui.h>

#include <gui/gui-util.h>
#include <util/Config.h>

void PlayerModule::Draw()
{
    ConfigWidget(Config::cfgGodModEnable, "Enables god mode. (No income damage); May not work with some types of damage.");
}

std::string PlayerModule::GetName()
{
    return "Player";
}