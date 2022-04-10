#include <pch.h>
#include "gui-util.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <cheat-base/util.h>
#include <shellapi.h>

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

bool InputHotkey(const char* label, Hotkey* hotkey, bool clearable)
{
    char hotkeyBuffer[50];

    auto hotkeyString = std::string(*hotkey);
    memcpy(hotkeyBuffer, hotkeyString.c_str(), hotkeyString.size() + 1);

    bool changed = false;

    if (clearable) {
        char labelBuffer[128];
        std::snprintf(labelBuffer, 128, "Clear ## %s_1", label);

        if (ImGui::Button(labelBuffer, ImVec2(75, 0)))
        {
            *hotkey = Hotkey();
            changed = true;
        }
        ImGui::SameLine();
    }

    changed = ImGui::HotkeyWidget(label, *hotkey, ImVec2(200, 0)) || changed;

    return changed;
}

bool InputPath(const char* label, std::filesystem::path* buffer, bool folder, const char* filter)
{
    bool changed = false;
    ImGui::PushID(label);
    if (ImGui::Button("Browse"))
    {
        auto value = folder ? util::SelectDirectory(label) : util::SelectFile(filter, label);
        if (value)
        {
            *buffer = *value;
            changed = true;
        }
    }
    ImGui::SameLine();
    changed |= ImGui::InputText(label, (char*)buffer->c_str(), buffer->string().capacity());

    ImGui::PopID();
    return changed;
}

#define ShowDesc(msg) if (desc != nullptr) { ImGui::SameLine(); HelpMarker(msg); }

struct ActiveInfo 
{
    void* valuePtr;
    bool changed;
};

static ActiveInfo prev;
static ActiveInfo current;

static bool IsValueChanged(void* valuePtr, bool result) 
{
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


bool ConfigWidget(const char* label, config::field::BaseField<bool>& field, const char* desc)
{
    bool result = ImGui::Checkbox(label, field.valuePtr());

    if (result)
        field.Check();

    ShowDesc(desc);
    
    return result;
}

bool ConfigWidget(const char* label, config::field::BaseField<int>& field, int step, int start, int end, const char* desc)
{
    bool result = false;

    if (start == end)
        result = ImGui::InputInt(label, field.valuePtr(), step);
    else
        result = ImGui::DragInt(label, field.valuePtr(), (float)step, start, end);

    if (IsValueChanged(field.valuePtr(), result))
        field.Check();

    ShowDesc(desc);

    return result;
}

bool ConfigWidget(const char* label, config::field::BaseField<float>& field, float step, float start, float end, const char* desc)
{
    bool result = false;

    if (start == end)
        result = ImGui::InputFloat(label, field.valuePtr(), step);
    else
        result = ImGui::DragFloat(label, field.valuePtr(), step, start, end);

    if (IsValueChanged(field.valuePtr(), result))
        field.Check();

    ShowDesc(desc);

    return result;
}

bool ConfigWidget(const char* label, config::field::HotkeyField& field, bool clearable, const char* desc)
{
    bool result = InputHotkey(label, field.valuePtr(), clearable);
    if (IsValueChanged(field.valuePtr(), result))
        field.Check();

    ShowDesc(desc);

    return result;
}

bool ConfigWidget(const char* label, config::field::BaseField<std::string>& field, const char* desc)
{
    bool result = ImGui::InputText(label, field.valuePtr());
    if (IsValueChanged(field.valuePtr(), result))
        field.Check();

    ShowDesc(desc);

    return result;
}

bool ConfigWidget(const char* label, config::field::BaseField<std::filesystem::path>& field, bool onlyDirectories, const char* filter, const char* desc)
{
    bool result = InputPath(label, field);
    if (IsValueChanged(field.valuePtr(), result))
        field.Check();

    ShowDesc(desc);

    return result;
}

bool ConfigWidget(config::field::BaseField<bool>& field, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, desc);
}

bool ConfigWidget(config::field::BaseField<int>& field, int step, int start, int end, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, step, start, end, desc);
}

bool ConfigWidget(config::field::BaseField<float>& field, float step, float start, float end, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, step, start, end, desc);
}

bool ConfigWidget(config::field::HotkeyField& field, bool clearable, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, clearable, desc);
}

bool ConfigWidget(config::field::BaseField<std::string>& field, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, desc);
}

bool ConfigWidget(config::field::BaseField<std::filesystem::path>& field, bool folder, const char* filter, const char* desc)
{
    return ConfigWidget(field.GetFriendlyName().c_str(), field, folder, filter, desc);
}

#undef ShowDesc

// https://github.com/ocornut/imgui/issues/1496#issuecomment-655048353

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

void AddUnderLine(ImColor col_)
{
	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();
	min.y = max.y;
	ImGui::GetWindowDrawList()->AddLine(min, max, col_, 1.0f);
}

void TextURL(const char* name_, const char* URL_, bool SameLineBefore_, bool SameLineAfter_)
{
	if (SameLineBefore_) { ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); }
	ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
	ImGui::Text(name_);
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			ShellExecute(0, 0, URL_, 0, 0, SW_SHOW);
		}
		AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
		ImGui::SetTooltip("Open in browser\n%s", URL_);
	}
	else
	{
		AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_Button]);
	}
	if (SameLineAfter_) { ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); }
}

// Modified version of: https://github.com/spirthack/CSGOSimple/blob/master/CSGOSimple/UI.cpp#L287 
bool ImGui::HotkeyWidget(const char* label, Hotkey& hotkey, const ImVec2& size)
{
    // Init ImGui
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    const ImVec2 item_size = ImGui::CalcItemSize(size, ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + item_size);
	const ImRect total_bb(window->DC.CursorPos, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

	ImGui::ItemSize(total_bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(total_bb, id))
		return false;


    const bool focus_requested = (ImGui::GetItemStatusFlags() & ImGuiItemStatusFlags_FocusedByTabbing) != 0 || g.NavActivateInputId == id;
    const bool hovered = ImGui::ItemHoverable(frame_bb, id);
	if (hovered) 
    {
		ImGui::SetHoveredID(id);
		g.MouseCursor = ImGuiMouseCursor_TextInput;
	}

	static Hotkey _initHotkey;
	static Hotkey _currHotkey;
    static Hotkey _prevHotkey;

	const bool user_clicked = hovered && io.MouseClicked[0];
	if (focus_requested || user_clicked) 
    {
		if (g.ActiveId != id) 
        {
			memset(io.MouseDown, 0, sizeof(io.MouseDown));
			memset(io.KeysDown, 0, sizeof(io.KeysDown));

            _initHotkey = hotkey;
            _currHotkey = Hotkey();
            _prevHotkey = Hotkey();
		}

		ImGui::SetActiveID(id, window);
		ImGui::FocusWindow(window);
	}
	else if (io.MouseClicked[0] && g.ActiveId == id)
    {
		ImGui::ClearActiveID();
	}

    bool valueChanged = false;

	if (g.ActiveId == id)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			ImGui::ClearActiveID();
            if (hotkey != _initHotkey)
            {
                hotkey = _initHotkey;
                valueChanged = true;
            }
		}
        else
        {
			ImGui::NavMoveRequestCancel();

			auto newHotkey = Hotkey::GetPressedHotkey();

            if (newHotkey.IsEmpty() && !_currHotkey.IsEmpty())
            {
                ImGui::ClearActiveID();
                valueChanged = false;
            } 
            else if (newHotkey - _prevHotkey)
			{
                _currHotkey = newHotkey;
                hotkey = newHotkey;
                valueChanged = true;
			}

            _prevHotkey = newHotkey;
        }

    }

	// Render
	// Select which buffer we are going to display. When ImGuiInputTextFlags_NoLiveEdit is Set 'buf' might still be the old value. We Set buf to NULL to prevent accidental usage from now on.

	char buf_display[128] = "Empty";

    const ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

	if ((g.ActiveId == id && !_currHotkey.IsEmpty()) || g.ActiveId != id)
		strcpy_s(buf_display, ((std::string)hotkey).c_str());
	else if (g.ActiveId == id) 
		strcpy_s(buf_display, "<Press a key>");

	const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + item_size.x, frame_bb.Min.y + item_size.y); // Not using frame_bb.Max because we have adjusted size
	ImVec2 render_pos = frame_bb.Min + style.FramePadding;
	ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect);
	//RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, GetColorU32(ImGuiCol_Text), style.ButtonTextAlign, &clip_rect);
	//draw_window->DrawList->AddText(g.Font, g.FontSize, render_pos, GetColorU32(ImGuiCol_Text), buf_display, NULL, 0.0f, &clip_rect);
    if (label_size.x > 0)
		ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

	return valueChanged;
}
