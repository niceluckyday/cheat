#pragma once

#include <imgui.h>
#include <cheat-base/config/Config.h>

bool ConfigWidget(const char* label, config::field::BaseField<bool>& field, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::field::BaseField<int>& field, int step = 1, int start = 0, int end = 0, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::field::BaseField<float>& field, float step = 1.0F, float start = 0, float end = 0, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::field::HotkeyField& field, bool clearable = true, const char* desc = nullptr);
bool ConfigWidget(const char* label, config::field::BaseField<std::string>& field, const char* desc = nullptr);

bool ConfigWidget(config::field::BaseField<bool>& field, const char* desc = nullptr);
bool ConfigWidget(config::field::BaseField<int>& field, int step = 1, int start = 0, int end = 0, const char* desc = nullptr);
bool ConfigWidget(config::field::BaseField<float>& field, float step = 1.0F, float start = 0, float end = 0, const char* desc = nullptr);
bool ConfigWidget(config::field::HotkeyField& field, bool clearable = true, const char* desc = nullptr);
bool ConfigWidget(config::field::BaseField<std::string>& field, const char* desc = nullptr);

void HelpMarker(const char* desc);
bool InputHotkey(const char* label, Hotkey* hotkey, bool clearable);

void BeginGroupPanel(const char* name, const ImVec2& size);
void EndGroupPanel();