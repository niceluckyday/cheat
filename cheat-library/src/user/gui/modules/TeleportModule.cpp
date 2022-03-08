#include "pch-il2cpp.h"
#include "TeleportModule.h"

#include <imgui.h>

#include <il2cpp-appdata.h>
#include <helpers.h>
#include <cheat/cheat.h>
#include <common/Config.h>
#include <common/util.h>
#include <gui/gui-util.h>

static void DrawEntities(const EntityFilter& filter) 
{
    auto entities = FindEntities(filter);
    if (entities.size() == 0)
    {
        ImGui::Text("Not found.");
        return;
    }
    
    for (const auto& entity : entities) 
    {
        ImGui::Text("Dist %.03fm", il2cppi_to_string(GetRelativePosition(entity)).c_str(), GetDistToAvatar(entity));
        ImGui::SameLine();
        auto label = string_format("Teleport ## %p", entity);
        if (ImGui::Button(label.c_str()))
        {
            auto rpos = GetRelativePosition(entity);
            auto apos = app::WorldShiftManager_GetAbsolutePosition(nullptr, rpos, nullptr);
            TeleportToPosition(apos);
        }
    }
}

void TeleportModule::Draw()
{
    BeginGroupPanel("Oculi teleport", ImVec2(-1, 0));
    {
        ConfigWidget(Config::cfgTeleportToOculi, "When key pressed, will teleport to nearest oculi if exists.");
        ConfigWidget(Config::cfgShowOculiInfo, "Showing short info about nearest oculi to info window.");
        if (ImGui::TreeNode("Oculies")) 
        {
            DrawEntities(GetFilterCrystalShell());
            ImGui::TreePop();
        }
    }
    EndGroupPanel();

    BeginGroupPanel("Chest teleport", ImVec2(-1, 0));
    {
        ConfigWidget(Config::cfgTeleportToChest, "When key pressed, will teleport to nearest chest if exists.");
        ConfigWidget(Config::cfgShowChestInfo, "Showing short info about nearest chest to info window.");
        if (ImGui::TreeNode("Chests"))
        {
            DrawEntities(GetFilterChest());
            ImGui::TreePop();
        }
    }
    EndGroupPanel();

    BeginGroupPanel("Map teleport", ImVec2(-1, 0));
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

        ConfigWidget(Config::cfgTeleportKey, true,
            "Key which you need have pressed while clicking to target location.");

        if (!Config::cfgMapTPEnable.GetValue())
            ImGui::EndDisabled();
    }
    EndGroupPanel();

}

std::string TeleportModule::GetName()
{
	return "Teleport";
}

static void DrawNearestEntityInfo(const char* prefix, const EntityFilter& filter) 
{
    auto nearestEntity = FindNearestEntity(filter);
    if (nearestEntity == nullptr) 
    {
        ImGui::Text(prefix); ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.1f, 0.1f, 1.0f), "not found");
        return;
    }
    ImGui::Text(prefix); ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), "%.3fm", GetDistToAvatar(nearestEntity));
}

void TeleportModule::DrawInfo()
{
    if (Config::cfgShowOculiInfo.GetValue()) 
        DrawNearestEntityInfo("Oculi", GetFilterCrystalShell());

    if (Config::cfgShowChestInfo.GetValue())
        DrawNearestEntityInfo("Chest", GetFilterChest());
}

bool TeleportModule::HasInfoToDraw()
{
    return Config::cfgShowOculiInfo.GetValue() || Config::cfgShowChestInfo.GetValue();
}
