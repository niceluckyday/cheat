#include "pch-il2cpp.h"
#include "renderer.h"

#include <iostream>
#include <vector>

#include <resource.h>
#include <util/dx11hook.h>
#include <util/Config.h>
#include <imgui.h>

#include "modules/SettingsModule.h"
#include "modules/TeleportModule.h"
#include "modules/DebugModule.h"
#include "modules/HotkeysModule.h"
#include "modules/ActiveFeaturesModule.h"
#include "modules/PlayerModule.h"
#include "modules/WorldModule.h"

#include <util/Logger.h>


static bool prevMouseActive = false;
static bool isShowMain = false;
static bool isBlockInteraction = true;

static ImFont* pFont;

static std::vector<IGUIModule*> modules = {
		new PlayerModule(),
		new WorldModule(),
        new TeleportModule(),
        new SettingsModule(),
		new HotkeysModule(),
		new DebugModule()
};

static ActiveFeaturesModule statusModule{};

bool GetResourceMemory(HINSTANCE hInstance, int resId, LPBYTE& pDest, DWORD& size);

void GuiRender();
void OnKeyUp(WPARAM key, short& ioFlag);

void InitRenderer(HMODULE hModule)
{
	LOG_DEBUG("Started ImGUI installing...");
    LPBYTE pFont = nullptr;
    DWORD dFontSize = 0;
	
    GetResourceMemory(hModule, IDR_RCDATA1, pFont, dFontSize);
    createOverlay(GuiRender, OnKeyUp, pFont, dFontSize);
}

void GuiRender() 
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
	if (!isShowMain)
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
        for (auto& mod: modules)
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

void CheckToggleHotkeys(short key)
{
	for (auto& field : Config::GetToggleFields()) 
	{
		if (field.GetHotkey()->IsPressed(key)) 
		{
			bool* value = field.GetValuePtr();
			*value = !*value;
			field.Check();
		}
	}
}

void OnKeyUp(WPARAM key, short& ioFlag) {
	if (Config::cfgMenuShowKey.GetValue().IsPressed((short)key)) 
	{
		isShowMain = !isShowMain;

		if (isShowMain) {
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

	if (!isShowMain)
		CheckToggleHotkeys((short)key);

	ioFlag = 0;
	if (isShowMain) 
	{
		ioFlag |= BType_SendImGUI;
		if (isBlockInteraction)
			ioFlag |= BType_Blocked;
	}
}

bool GetResourceMemory(HINSTANCE hInstance, int resId, LPBYTE& pDest, DWORD& size) {
	HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resId), RT_RCDATA);
	if (hResource) {
		HGLOBAL hGlob = LoadResource(hInstance, hResource);
		if (hGlob) {
			size = SizeofResource(hInstance, hResource);
			pDest = static_cast<LPBYTE>(LockResource(hGlob));
			if (size > 0 && pDest)
				return true;
		}
	}
	return false;
}
