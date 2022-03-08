#include "pch-il2cpp.h"
#include "PlayerModule.h"

#include <imgui.h>

#include <common/Config.h>
#include <gui/gui-util.h>

void PlayerModule::Draw()
{
    ConfigWidget(Config::cfgGodModEnable, "Enables god mode. (No income damage); May not work with some types of damage.");

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

    BeginGroupPanel("Infinite stamina", ImVec2(-1.0f, 0.0f));
    {
        ConfigWidget("Enable", Config::cfgInfiniteStaminaEnable, "Enables infinite stamina option.");
        ConfigWidget(Config::cfgISMovePacketMode, 
            "This mode prevent send to server packets with stamina cost actions,\nlike (swim, climb, sprint and so on).\nNOTE. This is can be more safe than standart method. But it is not tested.");
    }
    EndGroupPanel();

    ImGui::PopStyleVar();
}

std::string PlayerModule::GetName()
{
    return "Player";
}