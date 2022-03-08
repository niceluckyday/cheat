#include <pch-il2cpp.h>
#include "gui-util.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

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

static int HotkeyCallback(ImGuiInputTextCallbackData* data);

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

static int HotkeyCallback(ImGuiInputTextCallbackData* data) {
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

static bool IsValueChanged(void* valuePtr, bool result) {
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


bool ConfigWidget(const char* label, ConfigField<bool>& field, const char* desc) 
{
    bool result = ImGui::Checkbox(label, field.GetValuePtr());

    if (result)
        field.Check();

    ShowDesc(desc);
    
    return result;
}

bool ConfigWidget(const char* label, ConfigField<int>& field, int step, int start, int end, const char* desc)
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

bool ConfigWidget(const char* label, ConfigField<float>& field, float step, float start, float end, const char* desc)
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

bool ConfigWidget(const char* label, ConfigField<Hotkey>& field, bool clearable, const char* desc)
{
    bool result = InputHotkey(label, field.GetValuePtr(), clearable);
    if (IsValueChanged(field.GetValuePtr(), result))
        field.Check();

    ShowDesc(desc);

    return result;
}

bool ConfigWidget(const char* label, ConfigField<std::string>& field, const char* desc)
{
    bool result = ImGui::InputText(label, field.GetValuePtr());
    if (IsValueChanged(field.GetValuePtr(), result))
        field.Check();

    ShowDesc(desc);

    return result;
}

bool ConfigWidget(ConfigField<bool>& field, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, desc);
}

bool ConfigWidget(ConfigField<int>& field, int step, int start, int end, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, step, start, end, desc);
}

bool ConfigWidget(ConfigField<float>& field, float step, float start, float end, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, step, start, end, desc);
}

bool ConfigWidget(ConfigField<Hotkey>& field, bool clearable, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, clearable, desc);
}

bool ConfigWidget(ConfigField<std::string>& field, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, desc);;
}

#undef ShowDesc

static ImVector<ImRect> s_GroupPanelLabelStack;

void BeginGroupPanel(const char* name, const ImVec2& size)
{
    ImGui::BeginGroup();

    auto cursorPos = ImGui::GetCursorScreenPos();
    auto itemSpacing = ImGui::GetStyle().ItemSpacing;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    auto frameHeight = ImGui::GetFrameHeight();
    ImGui::BeginGroup();

    ImVec2 effectiveSize = size;
    if (size.x < 0.0f)
        effectiveSize.x = ImGui::GetContentRegionAvail().x;
    else
        effectiveSize.x = size.x;
    ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::TextUnformatted(name);
    auto labelMin = ImGui::GetItemRectMin();
    auto labelMax = ImGui::GetItemRectMax();
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
    ImGui::BeginGroup();

    //ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

    ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->WorkRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->InnerRect.Max.x -= frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow()->Size.x -= frameHeight;

    auto itemWidth = ImGui::CalcItemWidth();
    ImGui::PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));

    s_GroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));
}

void EndGroupPanel()
{
    ImGui::PopItemWidth();

    auto itemSpacing = ImGui::GetStyle().ItemSpacing;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    auto frameHeight = ImGui::GetFrameHeight();

    ImGui::EndGroup();

    //ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

    ImGui::EndGroup();

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

    ImGui::EndGroup();

    auto itemMin = ImGui::GetItemRectMin();
    auto itemMax = ImGui::GetItemRectMax();
    //ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

    auto labelRect = s_GroupPanelLabelStack.back();
    s_GroupPanelLabelStack.pop_back();

    ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
    ImRect frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
    labelRect.Min.x -= itemSpacing.x;
    labelRect.Max.x += itemSpacing.x;
    for (int i = 0; i < 4; ++i)
    {
        switch (i)
        {
            // left half-plane
        case 0: ImGui::PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true); break;
            // right half-plane
        case 1: ImGui::PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true); break;
            // top
        case 2: ImGui::PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true); break;
            // bottom
        case 3: ImGui::PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true); break;
        }

        ImGui::GetWindowDrawList()->AddRect(
            frameRect.Min, frameRect.Max,
            ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)),
            halfFrame.x);

        ImGui::PopClipRect();
    }

    ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->WorkRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->InnerRect.Max.x += frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow()->Size.x += frameHeight;

    ImGui::Dummy(ImVec2(0.0f, 0.0f));

    ImGui::EndGroup();
}