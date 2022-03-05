#pragma once
#include <string>
#include <vector>

#include <simple-ini.hpp>

#include <common/Hotkey.h>

template<class FieldType>
class ConfigField {
public:
	using OnChangeCallback = void (*)(ConfigField<FieldType>* field);

	void operator=(const FieldType& other) {
		*field = other;
		*prevValue = other;
		callback(this);
	}

	ConfigField(const std::string friendlyName, const std::string section, const std::string name, FieldType defaultValue, OnChangeCallback callback)
		: userName(friendlyName), section(section), name(name), field(new FieldType(defaultValue)), prevValue(new FieldType(defaultValue)), callback(callback)
	{ }

	std::string GetFriendlyName() {
		return userName;
	}

	std::string GetName() {
		return name;
	}

	std::string GetSection() {
		return section;
	}

	FieldType GetValue() {
		return *field;
	}

	FieldType* GetValuePtr() {
		return field;
	}

	void Check() {
		if (callback == nullptr || *prevValue == *field)
			return;

		*prevValue = *field;
		callback(this);
	}

private:
	OnChangeCallback callback;
	FieldType* field;
	FieldType* prevValue;

	std::string name;
	std::string section;
	std::string userName;

};

class ToggleConfigField : public ConfigField<bool> 
{
public:

	using OnChangeCallbackHotkey = void (*)(ConfigField<Hotkey>* field);

	ToggleConfigField(const std::string friendlyName, const std::string section, const std::string name, bool defaultValue, 
		OnChangeCallback callback, OnChangeCallbackHotkey hotkeyCallback)
		: ToggleConfigField(friendlyName, section, name, defaultValue, Hotkey(0, 0), callback, hotkeyCallback) { }

	ToggleConfigField(const std::string friendlyName, const std::string section, const std::string name, bool defaultValue, 
		Hotkey hotkey, OnChangeCallback callback, OnChangeCallbackHotkey hotkeyCallback)
		: ConfigField<bool>(friendlyName, section, name, defaultValue, callback), 
		hotkeyField(ConfigField<Hotkey>(friendlyName, "Hotkeys", name, hotkey, hotkeyCallback)) { }

	Hotkey* GetHotkey() {
		return hotkeyField.GetValuePtr();
	}

	ConfigField<Hotkey> GetHotkeyField() {
		return hotkeyField;
	}

private:
	ConfigField<Hotkey> hotkeyField;
};

#define NoSaveField(type, field, uname, section, def) inline static ConfigField<type> cfg## field = { uname, section, #field, def, nullptr }
#define Field(type, field, uname, section, def) inline static ConfigField<type> cfg## field = { uname, section, #field, def, Config::OnChangeValue }
#define ToggleField(field, uname, section, defBool) inline static ToggleConfigField cfg## field = { uname, section, #field, defBool, Config::OnChangeValue, Config::OnChangeValue }

class Config {

private:
	template<class FieldType>
	static void OnChangeValue(ConfigField<FieldType>* field) {
		SetValue(field->GetSection(), field->GetName(), field->GetValue());
		Save();
	}

public:
	
	// Note. If you adding field, don't forget add it into Config::Init()
	// Note. ToggleField hotkey will be automaticaly added to gui. See gui/modules/HotkeysModule.h.

	// Player cheats
	ToggleField(GodModEnable,          "God mode", "Player", false);
	ToggleField(InfiniteStaminaEnable, "Infinite stamina", "Player", false);
	ToggleField(InstantBowEnable,      "Instant bow", "Player", false);
	ToggleField(NoCDEnable,            "No ability CD", "Player", false);
	ToggleField(NoGravityEnable,       "No gravity", "Player", false);
	ToggleField(MoveSpeedhackEnable,   "Move speedhack", "Player", false);

	// World 
	ToggleField(UnlockWaypointsEnable, "Unlock waypoints", "World", false);
	ToggleField(DummyEnemiesEnabled,   "Dummy enemies", "World", false);

	// Teleportation
	ToggleField(  MapTPEnable,         "Map teleport",         "Teleport", true);
	Field(float,  TeleportHeight,      "Teleport height",      "Teleport", 500.0f);
	Field(Hotkey, TeleportKey,         "Teleport key",         "Teleport", Hotkey('T', 0));
	Field(bool,   UseOfflineTeleport,  "Use offline teleport", "Teleport", false);

	// Logging										       
	Field(bool, ConsoleLogEnabled,     "Console logging", "Logging", true);
	Field(bool, FileLogEnabled,        "File logging",    "Logging", false);

	// General							
	Field(bool,   DisableMhyprot,      "Disable protection",    "DLLUtil", true);
	Field(Hotkey, MenuShowKey,         "Show cheat menu key",   "DLLUtil", Hotkey(VK_F1, 0));
	Field(bool,   MoveStatusWindow,    "Move 'Active features'","DLLUtil", false);

	// Not save configs
	NoSaveField(bool, CheatWindowShowed, "", "", false);

	static void Init(const std::string configFile);

	static void Save();

	static std::vector<ToggleConfigField> GetToggleFields();

private:

	inline static CSimpleIni m_INIFile{};
	inline static std::vector<ToggleConfigField> toggleFields{};

	static void SetValue(std::string section, std::string name, bool value);
	static void SetValue(std::string section, std::string name, int value);
	static void SetValue(std::string section, std::string name, float value);
	static void SetValue(std::string section, std::string name, char* value);
	static void SetValue(std::string section, std::string name, Hotkey value);

	static void LoadFieldValue(ConfigField<bool>& field);
	static void LoadFieldValue(ConfigField<int>& field);
	static void LoadFieldValue(ConfigField<float>& field);
	static void LoadFieldValue(ConfigField<char*>& field);
	static void LoadFieldValue(ConfigField<Hotkey>& field);
	static void LoadFieldValue(ToggleConfigField& field);

};

#undef Field
#undef ToggleField