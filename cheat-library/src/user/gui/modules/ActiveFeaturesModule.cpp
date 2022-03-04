#include "pch-il2cpp.h"
#include "ActiveFeaturesModule.h"

#include <imgui.h>

#include <gui/gui-util.h>
#include <util/Config.h>

void ActiveFeaturesModule::Draw()
{

    static ImGuiTableFlags tabFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

    if (ImGui::BeginTable("activesTable", 1, tabFlags)) {

        ImGui::TableSetupColumn("Active features");
        ImGui::TableHeadersRow();

        int row = 0;

#define DrawLine(field) if (field ##.GetValue()) { \
            ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); \
            ImGui::TextUnformatted(field ##.GetFriendlyName().c_str()); \
            ImU32 row_bg_color = ImGui::GetColorU32(ImVec4(0.2f + row * 0.05f, 0.1f, 0.1f, 0.85f)); \
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, row_bg_color); \
            row++; }

        DrawLine(Config::cfgGodModEnable);
        DrawLine(Config::cfgInfiniteStaminaEnable);
        DrawLine(Config::cfgInstantBowEnable);
        DrawLine(Config::cfgNoCDEnable);
        DrawLine(Config::cfgNoGravityEnable);
        DrawLine(Config::cfgMoveSpeedhackEnable);

        DrawLine(Config::cfgUnlockWaypointsEnable);
        DrawLine(Config::cfgDummyEnemiesEnabled);
#undef DrawLine

        ImGui::EndTable();
    }
}

std::string ActiveFeaturesModule::GetName()
{
    return "Active features";
}