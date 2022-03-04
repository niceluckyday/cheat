#include <pch-il2cpp.h>
#include "gui-util.h"

#include <string>
#include <iostream>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>


void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

int HotkeyCallback(ImGuiInputTextCallbackData* data);

struct HotkeyUD {
    Hotkey* hotkey;
    bool* changed;
};

bool InputHotkey(const char* label, Hotkey* hotkey, bool clearable) {
    char hotkeyBuffer[50];

    auto hotkeyString = std::string(*hotkey);
    memcpy(hotkeyBuffer, hotkeyString.c_str(), hotkeyString.size() + 1);

    bool changed = false;

    HotkeyUD userdata;
    userdata.hotkey = hotkey;
    userdata.changed = &changed;

    if (clearable) {
        char labelBuffer[128];
        std::snprintf(labelBuffer, 128, "Clear ## %s_1", label);

        if (ImGui::Button(labelBuffer, ImVec2(75, 0)))
        {
            *hotkey = Hotkey(0, 0);
            changed = true;
        }
        ImGui::SameLine();
    }

    ImGui::PushItemWidth(100);
    ImGui::InputText(label, hotkeyBuffer, 10, ImGuiInputTextFlags_CallbackAlways, HotkeyCallback, &userdata);
    ImGui::PopItemWidth();

    return changed;
}

int HotkeyCallback(ImGuiInputTextCallbackData* data) {
    data->SelectionStart = 0;
    data->SelectionEnd = 0;
    data->CursorPos = 0;

    auto userdata = (HotkeyUD*)data->UserData;;
    Hotkey* currentHotkey = userdata->hotkey;


    // Slow solution to find pressed keys.
    // Needs to rewrite... in future
    Hotkey newHotkey = Hotkey::GetPressedHotkey();
    if (*currentHotkey != newHotkey)
    {
        if ((newHotkey.GetAKey() != 0 && currentHotkey->GetAKey() != newHotkey.GetAKey()) ||
            (newHotkey.GetMKey() != 0 && currentHotkey->GetMKey() != newHotkey.GetMKey()))
        {
            *userdata->changed = true;
            *currentHotkey = newHotkey;
        }
    }

    auto hotkeyString = std::string(*currentHotkey);
    memcpy(data->Buf, hotkeyString.c_str(), hotkeyString.size());
    data->Buf[hotkeyString.size()] = 0;
    data->BufDirty = true;
    data->BufTextLen = hotkeyString.size();
    return 0;
}

#define ShowDesc(msg) if (desc != nullptr) { ImGui::SameLine(); HelpMarker(msg); }

struct ActiveInfo {
    void* valuePtr;
    bool changed;
};

static ActiveInfo prev;
static ActiveInfo current;

bool IsValueChanged(void* valuePtr, bool result) {
    if (ImGui::IsItemActivated()) {
        prev = current;
        current = { valuePtr, result };
        return false;
    }

    if (ImGui::IsItemActive()) {
        current.changed |= result;
        return false;
    }

    if (ImGui::IsItemDeactivated()) {
        auto item = (current.valuePtr == valuePtr) ? current : prev;
        return item.changed;
    }

    return result;
}


bool ConfigWidget(const char* label, ConfigField<bool> field, const char* desc) 
{
    bool result = ImGui::Checkbox(label, field.GetValuePtr());

    if (result)
        field.Check();

    ShowDesc(desc);
    
    return result;
}

bool ConfigWidget(const char* label, ConfigField<int> field, int step, int start, int end, const char* desc)
{
    bool result = false;

    if (start == end)
        result = ImGui::InputInt(label, field.GetValuePtr(), step);
    else
        result = ImGui::DragInt(label, field.GetValuePtr(), (float)step, start, end);

    if (IsValueChanged(field.GetValuePtr(), result))
        field.Check();

    ShowDesc(desc);

    return result;
}

bool ConfigWidget(const char* label, ConfigField<float> field, float step, float start, float end, const char* desc)
{
    bool result = false;

    if (start == end)
        result = ImGui::InputFloat(label, field.GetValuePtr(), step);
    else
        result = ImGui::DragFloat(label, field.GetValuePtr(), step, start, end);

    if (IsValueChanged(field.GetValuePtr(), result))
        field.Check();

    ShowDesc(desc);

    return result;
}

bool ConfigWidget(const char* label, ConfigField<Hotkey> field, bool clearable, const char* desc) 
{
    bool result = InputHotkey(label, field.GetValuePtr(), clearable);
    if (IsValueChanged(field.GetValuePtr(), result))
        field.Check();

    ShowDesc(desc);

    return result;
}

bool ConfigWidget(const char* label, ConfigField<std::string> field, const char* desc) 
{
    bool result = ImGui::InputText(label, field.GetValuePtr());
    if (IsValueChanged(field.GetValuePtr(), result))
        field.Check();

    ShowDesc(desc);

    return result;
}

bool ConfigWidget(ConfigField<bool> field, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, desc);
}

bool ConfigWidget(ConfigField<int> field, int step, int start, int end, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, step, start, end, desc);
}

bool ConfigWidget(ConfigField<float> field, float step, float start, float end, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, step, start, end, desc);
}

bool ConfigWidget(ConfigField<Hotkey> field, bool clearable, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, clearable, desc);
}

bool ConfigWidget(ConfigField<std::string> field, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, desc);;
}

#undef ShowDesc