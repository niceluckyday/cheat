#pragma once
#include <string>
#include <vector>

#include <simple-ini.hpp>

#include <common/Hotkey.h>
#include <common/Event.h>
#include <common/Logger.h>

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

	virtual bool Check()
	{
		if (callback == nullptr || *prevValue == *field)
			return false;

		*prevValue = *field;
		callback(this);
		return true;
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

	inline static TEvent<ToggleConfigField*> OnChangedEvent{};

	using OnChangeCallbackHotkey = void (*)(ConfigField<Hotkey>* field);
	using OnChangeCallback = void (*)(ConfigField<bool>* field);

	ToggleConfigField(const std::string friendlyName, const std::string section, const std::string name, bool defaultValue, 
		OnChangeCallback callback, OnChangeCallbackHotkey hotkeyCallback)
		: ToggleConfigField(friendlyName, section, name, defaultValue, Hotkey(0, 0), callback, hotkeyCallback) { }

	ToggleConfigField(const std::string friendlyName, const std::string section, const std::string name, bool defaultValue, 
		Hotkey hotkey, OnChangeCallback callback, OnChangeCallbackHotkey hotkeyCallback)
		: ConfigField<bool>(friendlyName, section, name, defaultValue, callback),
		hotkeyField(ConfigField<Hotkey>(friendlyName, "Hotkeys", name, hotkey, hotkeyCallback)) { }

	Hotkey* GetHotkey() 
	{
		return hotkeyField.GetValuePtr();
	}

	ConfigField<Hotkey> GetHotkeyField() 
	{
		return hotkeyField;
	}

	virtual bool Check() override {
		if (!ConfigField<bool>::Check())
			return false;

		OnChangedEvent(this);

		return true;
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
	
	//   Infinite stamina
	ToggleField(InfiniteStaminaEnable, "Infinite stamina", "Player", false);
	ToggleField(ISMovePacketMode,      "Move packet replacement", "Player", false);

	ToggleField(InstantBowEnable,      "Instant bow", "Player", false);
	ToggleField(NoCDEnable,            "No ability CD", "Player", false);
	ToggleField(NoGravityEnable,       "No gravity", "Player", false);
	ToggleField(MoveSpeedhackEnable,   "Move speedhack", "Player", false);

	// World 
	ToggleField(UnlockWaypointsEnable, "Unlock waypoints", "World", false);
	ToggleField(DumbEnemiesEnabled,    "Dumb enemies", "World", false);

	// Teleport to nearest oculi
	Field(Hotkey, TeleportToOculi,     "TP to oculi key",          "Teleport", Hotkey());
	Field(bool, ShowOculiInfo,      "Show nearest oculi info",  "Teleport", true);
	
	// Teleport to nearest chest
	Field(Hotkey, TeleportToChest,     "TP to chest key",          "Teleport", Hotkey());
	Field(bool, ShowChestInfo,         "Show nearest chest info",  "Teleport", true);
	Field(bool, ShowOnlyUnlockedChest, "Show only unlocked chest", "Teleport", true);

	// Teleportation
	ToggleField(  MapTPEnable,         "Map teleport",             "Teleport", true);
	Field(bool,   CalcHeight,          "Auto detect ground height","Teleport", true);
	Field(float,  TeleportHeight,      "Teleport height",          "Teleport", 500.0f);
	Field(Hotkey, TeleportKey,         "Teleport key",             "Teleport", Hotkey('T', 0));

	// Logging										       
	Field(bool, ConsoleLogEnabled,     "Console logging", "Logging", true);
	Field(bool, FileLogEnabled,        "File logging",    "Logging", false);

	// General							
	Field(bool,   DisableMhyprot,      "Disable protection",    "General", true);
	Field(Hotkey, MenuShowKey,         "Show cheat menu key",   "General", Hotkey(VK_F1, 0));
	
	Field(bool,   MoveStatusWindow,    "Move status window",    "General", false);
	Field(bool,   ShowStatusWindow,    "Show status window",    "General", true);

	Field(bool,   MoveInfoWindow,      "Move info window",      "General", false);
	Field(bool,   ShowInfoWindow,      "Show info window",      "General", true);

	// Not save configs
	NoSaveField(bool, CheatWindowShowed, "", "", false);
	NoSaveField(bool, OriginalInputBlock, "", "", true);

	static void Init(const std::string configFile);

	static void Save();

	static std::vector<ToggleConfigField*> GetToggleFields();

private:

	inline static CSimpleIni m_INIFile{};
	inline static std::vector<ToggleConfigField*> toggleFields{};

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