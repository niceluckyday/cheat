#include <pch-il2cpp.h>
#include "main-window.h"

#include <iostream>
#include <vector>

#include <imgui.h>

#include <common/Config.h>
#include <common/Logger.h>
#include <common/GlobalEvents.h>

#include "modules/IGUIModule.h"
#include "modules/SettingsModule.h"
#include "modules/TeleportModule.h"
#include "modules/DebugModule.h"
#include "modules/HotkeysModule.h"
#include "modules/StatusModule.h"
#include "modules/PlayerModule.h"
#include "modules/WorldModule.h"

static bool prevMouseActive = false;
static bool isShowMain = false;
static bool isBlockInteraction = true;

static std::vector<IGUIModule*> modules = {
        new PlayerModule(),
        new WorldModule(),
        new TeleportModule(),
        new SettingsModule(),
        new HotkeysModule(),
        new DebugModule()
};

static StatusModule statusModule{};

static void OnKeyUp(short key, bool& cancelled);

void InitializeWindow() 
{
    GlobalEvents::KeyUpEvent += FREE_METHOD_HANDLER(OnKeyUp);
}

void DrawWindow() 
{
    // Drawing status window
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;

    if (!Config::cfgMoveStatusWindow.GetValue())
        flags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;

    ImGui::Begin("Cheat status", 0, flags);

    statusModule.Draw();

    ImGui::End();

    static IGUIModule* current = modules[0];
    if (!Config::cfgCheatWindowShowed.GetValue())
        return;

    ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("CCGenshin (Callow's cheat)"))
    {
        ImGui::End();
        ImGui::PopFont();
        return;
    }

    ImGui::BeginGroup();

    ImGui::Checkbox("Block key/mouse", &isBlockInteraction);

    if (ImGui::BeginListBox("##listbox 2", ImVec2(175, -FLT_MIN)))
    {
        for (auto& mod : modules)
        {
            const bool is_selected = (current == mod);
            if (ImGui::Selectable(mod->GetName().c_str(), is_selected))
                current = mod;

            if (is_selected)
                ImGui::SetItemDefaultFocus();

        }
        ImGui::EndListBox();
    }

    ImGui::EndGroup();

    ImGui::SameLine();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::BeginChild("ChildR", ImVec2(0, 0), true, window_flags);

    ImGui::Text(current->GetName().c_str());
    ImGui::Separator();

    current->Draw();

    ImGui::EndChild();
    ImGui::PopStyleVar();

    ImGui::End();
}

bool NeedInput() 
{
    return Config::cfgCheatWindowShowed.GetValue();
}

static void OnKeyUp(short key, bool& cancelled)
{
    if (Config::cfgCheatWindowShowed.GetValue() && isBlockInteraction) {
        cancelled = true;
    }

    if (!Config::cfgMenuShowKey.GetValue().IsPressed(key))
        return;

    bool* windowShowed = Config::cfgCheatWindowShowed.GetValuePtr();
    *windowShowed = !*windowShowed;

    if (*windowShowed) {
        prevMouseActive = app::Cursor_get_visible(nullptr, nullptr);
        if (!prevMouseActive) {
            app::Cursor_set_visible(nullptr, true, nullptr);
            app::Cursor_set_lockState(nullptr, app::CursorLockMode__Enum::None, nullptr);
        }
    }
    else if (!prevMouseActive) {
        app::Cursor_set_visible(nullptr, false, nullptr);
        app::Cursor_set_lockState(nullptr, app::CursorLockMode__Enum::Locked, nullptr);
    }
}