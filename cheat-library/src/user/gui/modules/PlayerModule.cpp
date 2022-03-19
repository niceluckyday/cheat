#include "pch-il2cpp.h"
#include "PlayerModule.h"

#include <imgui.h>

#include <common/Config.h>
#include <gui/gui-util.h>

void PlayerModule::Draw()
{
    ConfigWidget(Config::cfgGodModEnable, "Enables god mode. (No income damage); May not work with some types of damage.");

    BeginGroupPanel("No clip", ImVec2(-1.0f, 0.0f));
    {
        ConfigWidget("Enable ## No clip", Config::cfgNoClipEnable, "Enables no clip.\nFor move use ('W', 'A', 'S', 'D', 'Space', 'Shift')");
        ConfigWidget(Config::cfgNoClipSpeed, 0.1, 2, 100, "No clip move speed.\nIt's not recommended to set value above 5.");
        ConfigWidget(Config::cfgNoClipCameraMove, "Move performing relative to camera direction. Not to avatar.");
    }
    EndGroupPanel();

    BeginGroupPanel("Infinite stamina", ImVec2(-1.0f, 0.0f));
    {
        ConfigWidget("Enable ## Infinite stamina", Config::cfgInfiniteStaminaEnable, "Enables infinite stamina option.");
        ConfigWidget(Config::cfgISMovePacketMode,
            "This mode prevent send to server packets with stamina cost actions,\nlike (swim, climb, sprint and so on).\nNOTE. This is can be more safe than standart method. But it is not tested.");
    }
    EndGroupPanel();

    BeginGroupPanel("No cooldown", ImVec2(-1.0f, 0.0f));
    {
        ConfigWidget(Config::cfgInstantBowEnable, "Disable cooldown of bow charge.");
        ConfigWidget(Config::cfgNoSkillCDEnable, "Disable skills cooldown.");
        ConfigWidget(Config::cfgNoSprintCDEnable, "Disable speed cooldown.");
    }
    EndGroupPanel();

    BeginGroupPanel("Mob vacum", ImVec2(-1.0f, 0.0f));
    {
        ConfigWidget("Enable ## MobVaccum", Config::cfgMobVaccumEnable, "Enables mob vacum.\n\
            Mob in specified radius will be moved front of player in specified distance.");

        ConfigWidget(Config::cfgMobVaccumInstantly, "Move performs instantly.");
        ConfigWidget(Config::cfgMobVaccumSpeed, 0.1, 1, 15, "If 'Instantly' not checked, regulate speed of move.");
        ConfigWidget(Config::cfgMobVaccumRadius, 1, 5, 150, "Radius of mob vacum work area.");
        ConfigWidget(Config::cfgMobVaccumDistance, 0.1, 0.5, 10, "Distance beetween player and monster to move.");
        ConfigWidget(Config::cfgMobVaccumOnlyTarget, "Mob vacum will be work only on entity targeted at the player.");
    }
    EndGroupPanel();

    BeginGroupPanel("Rapid fire", ImVec2(-1.0f, 0.0f));
    {
        ConfigWidget("Enable ## Rapid fire", Config::cfgRapidFire, "Enables rapid fire.\n\
            Rapid fire just multiply your attack count.\n\
            It's not well tested, and can be detected by anticheat.\n\
            So not recommend to you that in your main account.");

        ConfigWidget(Config::cfgRapidFireMultiplier, 1, 2, 1000, "Rapid fire multiply count.\n\
            For example: 100 means that whey you hit enemy, attack duplicates 100 times.\n\
            Be aware, big value can produce lags.");

        ConfigWidget(Config::cfgRapidFireOnePunch, "Calculate count of attack repeat to kill entity by one attack.\n\
            Due to that damage calculate on server side, that can be calculated incorrect count of attacks.");
    }
    EndGroupPanel();
}

std::string PlayerModule::GetName()
{
    return "Player";
}