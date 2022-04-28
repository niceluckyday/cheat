#pragma once

#include <imgui.h>
#include <filesystem>
#include <cheat-base/config/config.h>
#include <cheat-base/Hotkey.h>
#include <cheat-base/config/fields/ToggleHotkey.h>
#include <cheat-base/config/fields/Enum.h>

#define BLOCK_FOCUS() 

bool ConfigWidget(const char* label, config::Field<bool>& field, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::Field<int>& field, int step = 1, int start = 0, int end = 0, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::Field<float>& field, float step = 1.0F, float start = 0, float end = 0, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::Field<Hotkey>& field, bool clearable = true, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::Field<std::string>& field, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::Field<ImColor>& field, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::Field<config::ToggleHotkey>& field, const char* desc = nullptr, bool hotkey = false);

bool ConfigWidget(config::Field<bool>& field, const char* desc = nullptr);
bool ConfigWidget(config::Field<int>& field, int step = 1, int start = 0, int end = 0, const char* desc = nullptr);
bool ConfigWidget(config::Field<float>& field, float step = 1.0F, float start = 0, float end = 0, const char* desc = nullptr);
bool ConfigWidget(config::Field<Hotkey>& field, bool clearable = true, const char* desc = nullptr);
bool ConfigWidget(config::Field<std::string>& field, const char* desc = nullptr);
bool ConfigWidget(config::Field<ImColor>& field, const char* desc = nullptr);
bool ConfigWidget(config::Field<config::ToggleHotkey>& field, const char* desc = nullptr, bool hotkey = false);

void ShowHelpText(const char* text);
void HelpMarker(const char* desc);

bool InputHotkey(const char* label, Hotkey* hotkey, bool clearable);

// Thanks to https://gist.github.com/dougbinks/ef0962ef6ebe2cadae76c4e9f0586c69
void AddUnderLine(ImColor col_);
void TextURL(const char* name_, const char* URL_, bool SameLineBefore_, bool SameLineAfter_);

struct SelectData
{
	bool toggle;
	bool changed;
};

bool BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(-1, 0), bool node = false, SelectData* selectData = nullptr);
void EndGroupPanel();

namespace ImGui
{
	bool HotkeyWidget(const char* label, Hotkey& hotkey, const ImVec2& size = ImVec2(0, 0));
	bool PushStyleColorWithContrast(ImU32 backGroundColor, ImGuiCol foreGroundColor, ImU32 invertedColor, float maxContrastRatio);
}

float CalcWidth(const std::string_view& view);

template <typename T>
float GetMaxEnumWidth()
{
	constexpr auto names = magic_enum::enum_names<T>();
	auto maxComboName = std::max_element(names.begin(), names.end(),
		[](const auto& a, const auto& b) { return CalcWidth(a) < CalcWidth(b); });
	return CalcWidth(*maxComboName);
}

template <typename T>
bool ComboEnum(const char* label, T* currentValue)
{
	auto name = magic_enum::enum_name(*currentValue);
	auto& current = *currentValue;
	bool result = false;
	static auto width = GetMaxEnumWidth<T>();

	ImGui::SetNextItemWidth(width);
	if (ImGui::BeginCombo(label, name.data()))
	{
		for (auto& entry : magic_enum::enum_entries<T>())
		{
			bool is_selected = (name == entry.second);
			if (ImGui::Selectable(entry.second.data(), is_selected))
			{
				current = entry.first;
				result = true;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	return result;
}

template <typename T>
bool ConfigWidget(const char* label, config::Field<config::Enum<T>>& field, const char* desc = nullptr)
{
	bool result = false;
	if (ComboEnum(label, &field.value()))
	{
		field.FireChanged();
		result = true;
	}
	
	if (desc != nullptr) { ImGui::SameLine(); HelpMarker(desc); };
	return result;
}

template <typename T>
bool ConfigWidget(config::Field<config::Enum<T>>& field, const char* desc = nullptr)
{
	return ConfigWidget(field.friendName().c_str(), field, desc);
}