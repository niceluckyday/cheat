#pragma once

#include <common/Config.h>

bool ConfigWidget(const char* label, ConfigField<bool> field, const char* desc = nullptr);
bool ConfigWidget(const char* label, ConfigField<int> field, int step = 1, int start = 0, int end = 0, const char* desc = nullptr);
bool ConfigWidget(const char* label, ConfigField<float> field, float step = 1.0F, float start = 0, float end = 0, const char* desc = nullptr);
bool ConfigWidget(const char* label, ConfigField<Hotkey> field, bool clearable = true, const char* desc = nullptr);
bool ConfigWidget(const char* label, ConfigField<std::string> field, const char* desc = nullptr);

bool ConfigWidget(ConfigField<bool> field, const char* desc = nullptr);
bool ConfigWidget(ConfigField<int> field, int step = 1, int start = 0, int end = 0, const char* desc = nullptr);
bool ConfigWidget(ConfigField<float> field, float step = 1.0F, float start = 0, float end = 0, const char* desc = nullptr);
bool ConfigWidget(ConfigField<Hotkey> field, bool clearable = true, const char* desc = nullptr);
bool ConfigWidget(ConfigField<std::string> field, const char* desc = nullptr);

void HelpMarker(const char* desc);
bool InputHotkey(const char* label, Hotkey* hotkey, bool clearable);