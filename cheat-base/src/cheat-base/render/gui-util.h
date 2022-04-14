#pragma once

#include <imgui.h>
#include <filesystem>
#include <cheat-base/config/Config.h>
#include <cheat-base/Hotkey.h>
#include <cheat-base/config/field/EnumField.h>
#include <cheat-base/config/field/ColorField.h>

bool ConfigWidget(const char* label, config::field::BaseField<bool>& field, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::field::BaseField<int>& field, int step = 1, int start = 0, int end = 0, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::field::BaseField<float>& field, float step = 1.0F, float start = 0, float end = 0, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::field::HotkeyField& field, bool clearable = true, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::field::BaseField<std::string>& field, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::field::BaseField<std::filesystem::path>& field, bool onlyDirectories = false, const char* filter = nullptr, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::field::ColorField& field, const char* desc = nullptr);

bool ConfigWidget(config::field::BaseField<bool>& field, const char* desc = nullptr);
bool ConfigWidget(config::field::BaseField<int>& field, int step = 1, int start = 0, int end = 0, const char* desc = nullptr);
bool ConfigWidget(config::field::BaseField<float>& field, float step = 1.0F, float start = 0, float end = 0, const char* desc = nullptr);
bool ConfigWidget(config::field::HotkeyField& field, bool clearable = true, const char* desc = nullptr);
bool ConfigWidget(config::field::BaseField<std::string>& field, const char* desc = nullptr);
bool ConfigWidget(config::field::BaseField<std::filesystem::path>& field, bool onlyDirectories = false, const char* filter = nullptr, const char* desc = nullptr);
bool ConfigWidget(config::field::ColorField& field, const char* desc = nullptr);

void HelpMarker(const char* desc);

bool InputHotkey(const char* label, Hotkey* hotkey, bool clearable);
bool InputPath(const char* label, std::filesystem::path* buffer, bool onlyDirectories = false, const char* filter = nullptr);

// Thanks to https://gist.github.com/dougbinks/ef0962ef6ebe2cadae76c4e9f0586c69
void AddUnderLine(ImColor col_);
void TextURL(const char* name_, const char* URL_, bool SameLineBefore_, bool SameLineAfter_);

void BeginGroupPanel(const char* name, const ImVec2& size);
void EndGroupPanel();

bool IsValueChanged(void* valuePtr, bool result);

namespace ImGui
{
	bool HotkeyWidget(const char* label, Hotkey& hotkey, const ImVec2& size = ImVec2(0, 0));
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
bool ConfigWidget(const char* label, config::field::EnumField<T>& field, const char* desc = nullptr)
{
	bool result = false;
	if (ComboEnum(label, field.valuePtr()))
	{
		field.Check();
		result = true;
	}
	
	if (desc != nullptr) { ImGui::SameLine(); HelpMarker(desc); };
	return result;
}

template <typename T>
bool ConfigWidget(config::field::EnumField<T>& field, const char* desc = nullptr)
{
	return ConfigWidget(field.GetFriendlyName().c_str(), field, desc);
}