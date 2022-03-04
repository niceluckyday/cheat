#include "pch-il2cpp.h"
#include "TeleportModule.h"

#include <il2cpp-appdata.h>
#include <imgui.h>

#include <gui/gui-util.h>
#include <util/Config.h>

void TeleportModule::Draw()
{
    ConfigWidget(Config::cfgMapTPEnable,
        "Enable teleportation to mark functionality.\n" \
        "Usage: \n" \
        "\t1. Open map.\n" \
        "\t2. Click LMB to any place on map with pressed [Teleport key].\n" \
        "\tDone. You have been teleported to selected location."
    );

    if (!Config::cfgMapTPEnable.GetValue())
        ImGui::BeginDisabled();

    ConfigWidget(Config::cfgTeleportHeight, 1.0F, 200.0F, 800.0F,
        "If cheat cannot get ground height of target location, it will teleport you to specified here height.");

    // ConfigWidget("Offline TP", Config::cfgUseOfflineTP,
    //    "Don't notify server about teleportation.\n It's can be more unsecure than default variant.");

    ConfigWidget(Config::cfgTeleportKey, true, 
        "Key which you need have pressed while clicking to target location.");

    if (!Config::cfgMapTPEnable.GetValue())
        ImGui::EndDisabled();
}

std::string TeleportModule::GetName()
{
	return "Teleport";
}