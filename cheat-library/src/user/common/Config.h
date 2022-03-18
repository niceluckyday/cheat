#pragma once
#include <string>
#include <vector>

#include <gcclib/simple-ini.hpp>
#include <gcclib/Logger.h>

#include <common/Hotkey.h>
#include <common/Event.h>

class ConfigFieldHeader
{
public:
	ConfigFieldHeader(const std::string friendlyName, const std::string section, const std::string name, const uint32_t flags);

	std::string GetFriendlyName() const;
	std::string GetName() const;
	std::string GetSection() const;
	uint32_t GetFlags() const;

	template<typename flagEnumT>
	bool HasFlag(flagEnumT flag) const 
	{ 
		return flags & static_cast<uint32_t>(flag); 
	};

private:
	std::string name;
	std::string section;
	std::string userName;
	uint32_t flags;
};


template<class FieldType>
class ConfigField : public ConfigFieldHeader
{
public:
	using OnChangeCallback = void (*)(ConfigField<FieldType>* field);

	void operator=(const FieldType& other) 
	{
		*field = other;
		*prevValue = other; 
		callback(this);
	}

	ConfigField(const std::string friendlyName, const std::string section, const std::string name, const uint32_t flags,
		FieldType defaultValue, OnChangeCallback callback)
		: ConfigFieldHeader(friendlyName, section, name, flags),
		field(new FieldType(defaultValue)), prevValue(new FieldType(defaultValue)), 
		callback(callback)
	{ }

	FieldType GetValue() const 
	{
		return *field;
	}

	FieldType* GetValuePtr() const 
	{
		return field;
	}

	operator FieldType() const {
		return GetValue();
	}

	operator FieldType* () const {
		return GetValuePtr();
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
};


enum class ConfigFieldFlag
{
	None = 0,
	ToggleField = 1,
	NeedToShowStatus = 2
};

// Toggle config field, need for 
class ToggleConfigField : public ConfigField<bool> 
{
public:

	inline static TEvent<ToggleConfigField*> OnChangedEvent {};

	using OnChangeCallbackHotkey = void (*)(ConfigField<Hotkey>* field);
	using OnChangeCallback = void (*)(ConfigField<bool>* field);

	ToggleConfigField(const std::string friendlyName, const std::string section, const std::string name, const uint32_t flags,
		bool defaultValue, OnChangeCallback callback, OnChangeCallbackHotkey hotkeyCallback);

	ToggleConfigField(const std::string friendlyName, const std::string section, const std::string name, const uint32_t flags, 
		bool defaultValue, Hotkey hotkey, OnChangeCallback callback, OnChangeCallbackHotkey hotkeyCallback);

	Hotkey* GetHotkey();
	ConfigField<Hotkey> GetHotkeyField();
	virtual bool Check() override;

private:
	ConfigField<Hotkey> hotkeyField;
};

#define NoSaveField(type, field, uname, section, def) inline static ConfigField<type> cfg## field = { uname, section, #field, 0, def, nullptr }

#define Field(type, field, uname, section, def) inline static ConfigField<type> cfg## field = { uname, section, #field, 0, def, Config::OnChangeValue }
#define ToggleField(field, uname, section, defBool, flags) inline static ToggleConfigField cfg## field = { uname, section, #field, static_cast<uint32_t>(flags), defBool, Config::OnChangeValue, Config::OnChangeValue }

class Config {

private:
	template<class FieldType>
	static void OnChangeValue(ConfigField<FieldType>* field) 
	{
		SetValue(field->GetSection(), field->GetName(), field->GetValue());
		Save();
	}

public:
	
	// Note. If you adding field, don't forget add it into Config::Init()
	// Note. ToggleField hotkey will be automaticaly added to gui. See gui/modules/HotkeysModule.h.

	// Player cheats
	ToggleField(GodModEnable,          "God mode", "Player", false, ConfigFieldFlag::NeedToShowStatus);
	
	//   Infinite stamina
	ToggleField(InfiniteStaminaEnable, "Infinite stamina", "Player", false, ConfigFieldFlag::NeedToShowStatus);
	ToggleField(ISMovePacketMode,      "Move packet replacement", "Player", false, ConfigFieldFlag::None);

	//   Rapid fire
	ToggleField(RapidFire,             "Rapid fire",     "Player", false, ConfigFieldFlag::NeedToShowStatus);
	Field(int,   RapidFireMultiplier,  "Multiplier",     "Player", 2);
	Field(bool,  RapidFireOnePunch,    "One punch",      "Player", false);

	//   No coldown
	ToggleField(InstantBowEnable,      "Instant bow",    "Player", false, ConfigFieldFlag::NeedToShowStatus);
	ToggleField(NoSkillCDEnable,       "No ability CD",  "Player", false, ConfigFieldFlag::NeedToShowStatus);
	ToggleField(NoSprintCDEnable,      "No sprint CD",   "Player", false, ConfigFieldFlag::None);
	ToggleField(NoGravityEnable,       "No gravity",     "Player", false, ConfigFieldFlag::NeedToShowStatus);
	ToggleField(MoveSpeedhackEnable,   "Move speedhack", "Player", false, ConfigFieldFlag::None);

	//   Mob vaccum
	ToggleField( MobVaccumEnable,      "Mob vacum",      "Player", false, ConfigFieldFlag::NeedToShowStatus);
	Field(bool,  MobVaccumInstantly,   "Instantly",      "Player", false);
	Field(float, MobVaccumSpeed,       "Vacum speed",    "Player", 2.0f);
	Field(float, MobVaccumDistance,    "Front distance", "Player", 1.5f);
	Field(float, MobVaccumRadius,      "Vacum radius",   "Player", 60.0f);
	Field(bool,  MobVaccumOnlyTarget,  "Only targeted",  "Player", true);

	// World 
	ToggleField(UnlockWaypointsEnable, "Unlock waypoints", "World", false, ConfigFieldFlag::None);
	ToggleField(DumbEnemiesEnabled,    "Dumb enemies", "World", false, ConfigFieldFlag::NeedToShowStatus);

	ToggleField(AutoTalkEnabled,	   "Auto talk", "Dialog", false, ConfigFieldFlag::NeedToShowStatus);

	// Teleport to nearest oculi
	Field(Hotkey, TeleportToOculi,     "TP to oculi key",          "Teleport", Hotkey());
	Field(bool, ShowOculiInfo,         "Show nearest oculi info",  "Teleport", true);
	
	// Teleport to nearest chest
	Field(Hotkey, TeleportToChest,     "TP to chest key",          "Teleport", Hotkey());
	Field(bool, ShowChestInfo,         "Show nearest chest info",  "Teleport", true);
	Field(bool, ShowOnlyUnlockedChest, "Show only unlocked chest", "Teleport", true);

	// Teleportation
	ToggleField(  MapTPEnable,         "Map teleport",             "Teleport", true, ConfigFieldFlag::None);
	Field(bool,   CalcHeight,          "Auto detect ground height","Teleport", true);
	Field(float,  TeleportHeight,      "Teleport height",          "Teleport", 500.0f);
	Field(Hotkey, TeleportKey,         "Teleport key",             "Teleport", Hotkey('T', 0));

	// Logging										       
	Field(bool, ConsoleLogEnabled,     "Console logging", "Logging", true);
	Field(bool, FileLogEnabled,        "File logging",    "Logging", false);

	Field(bool, PacketCapturing,       "Packet capturing",      "Network", false);
	Field(bool, PacketManipulation,    "Packet manipulation",   "Network", false);

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

	static std::vector<ConfigFieldHeader*> GetFields();
	static std::vector<ToggleConfigField*> GetToggleFields();

private:

	inline static CSimpleIni m_INIFile{};
	inline static std::vector<ConfigFieldHeader*> fields{};
	inline static std::vector<ToggleConfigField*> toggleFields{};

	static void LoadFieldValue(ConfigField<char*>& field);
	static void SetValue(std::string section, std::string name, char* value);

	static void LoadFieldValue(ConfigField<float>& field);
	static void SetValue(std::string section, std::string name, float value);

	static void LoadFieldValue(ConfigField<bool>& field);
	static void SetValue(std::string section, std::string name, bool value);

	static void LoadFieldValue(ConfigField<int>& field);
	static void SetValue(std::string section, std::string name, int value);

	static void LoadFieldValue(ConfigField<Hotkey>& field);
	static void SetValue(std::string section, std::string name, Hotkey value);
	
	static void LoadFieldValue(ToggleConfigField& field);
};

#undef Field
#undef ToggleField
#undef NoSaveField

