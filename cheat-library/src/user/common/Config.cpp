#include "pch-il2cpp.h"
#include "Config.h"

#include <sstream>

#include <gcclib/Logger.h>
#include <gcclib/util.h>

static bool changed = false;

static std::string filename;

#define LoadField(field) LoadFieldValue(field);\
						 fields.push_back(&field);\
                         if (field.HasFlag(ConfigFieldFlag::ToggleField)) toggleFields.push_back(reinterpret_cast<ToggleConfigField*>(&field))

void Config::Init(const std::string configFile)
{
	filename = configFile;

	auto status = m_INIFile.LoadFile(configFile.c_str());
	if (status < 0)
		LOG_ERROR("Failed to load config file.");

	LoadField(cfgGodModEnable);
	LoadField(cfgInfiniteStaminaEnable);
	LoadField(cfgISMovePacketMode);
	LoadField(cfgMoveSpeedhackEnable);

	LoadField(cfgRapidFire);
	LoadField(cfgRapidFireMultiplier);
	LoadField(cfgRapidFireOnePunch);

	LoadField(cfgMobVaccumEnable);
	LoadField(cfgMobVaccumInstantly);
	LoadField(cfgMobVaccumDistance);
	LoadField(cfgMobVaccumSpeed);
	LoadField(cfgMobVaccumRadius);
	LoadField(cfgMobVaccumOnlyTarget);

	LoadField(cfgInstantBowEnable);
	LoadField(cfgNoSprintCDEnable);
	LoadField(cfgNoSkillCDEnable);
	LoadField(cfgNoGravityEnable);

	LoadField(cfgUnlockWaypointsEnable);
	LoadField(cfgDumbEnemiesEnabled);

	LoadField(cfgAutoLootEnabled);
	LoadField(cfgAutoLootDelayTime);
	LoadField(cfgAutoLootCustomRangeEnabled);
	LoadField(cfgAutoLootCustomRange);

	LoadField(cfgAutoTalkEnabled);

	LoadField(cfgMapTPEnable);
	LoadField(cfgTeleportHeight);
	LoadField(cfgTeleportKey);

	LoadField(cfgShowOculiInfo);
	LoadField(cfgTeleportToOculi);

	LoadField(cfgShowChestInfo);
	LoadField(cfgTeleportToChest);

	LoadField(cfgPacketCapturing);
	LoadField(cfgPacketManipulation);

	LoadField(cfgDisableMhyprot);
	LoadField(cfgConsoleLogEnabled);
	LoadField(cfgFileLogEnabled);

	LoadField(cfgMoveStatusWindow);
	LoadField(cfgShowStatusWindow);

	LoadField(cfgMoveInfoWindow);
	LoadField(cfgShowInfoWindow);
}

std::vector<ConfigFieldHeader*> Config::GetFields()
{
	return fields;
}

std::vector<ToggleConfigField*> Config::GetToggleFields()
{
	return toggleFields;
}

void Config::Save()
{
	auto status = m_INIFile.SaveFile(filename.c_str());
	if (status < 0)
		LOG_ERROR("Failed to save changes to config.");
}

void Config::LoadFieldValue(ConfigField<char*>& field)
{
	field = (char*)m_INIFile.GetValue(field.GetSection().c_str(), field.GetName().c_str(), field.GetValue());
}

void Config::SetValue(std::string section, std::string name, char* value) 
{
	m_INIFile.SetValue(section.c_str(), name.c_str(), value);
}


void Config::LoadFieldValue(ConfigField<float>& field)
{
	field = (float)m_INIFile.GetDoubleValue(field.GetSection().c_str(), field.GetName().c_str(), field.GetValue());
}

void Config::SetValue(std::string section, std::string name, float value) 
{
	m_INIFile.SetDoubleValue(section.c_str(), name.c_str(), (double)value);
}


void Config::LoadFieldValue(ConfigField<bool>& field)
{
	field = m_INIFile.GetBoolValue(field.GetSection().c_str(), field.GetName().c_str(), field.GetValue());
}

void Config::SetValue(std::string section, std::string name, bool value) 
{
	m_INIFile.SetBoolValue(section.c_str(), name.c_str(), value);
}


void Config::LoadFieldValue(ConfigField<int>& field)
{
	field = (int)m_INIFile.GetLongValue(field.GetSection().c_str(), field.GetName().c_str(), field.GetValue());
}

void Config::SetValue(std::string section, std::string name, int value) 
{
	m_INIFile.SetLongValue(section.c_str(), name.c_str(), (long)value);
}


void Config::LoadFieldValue(ConfigField<Hotkey>& field)
{
	const char* rawValue = m_INIFile.GetValue(field.GetSection().c_str(), field.GetName().c_str(), nullptr);
	if (rawValue == nullptr)
		return;

	std::stringstream stream;
	stream << rawValue;
	int mKey = 0, aKey = 0;
	stream >> mKey >> aKey;
	field = Hotkey(mKey, aKey);
}

void Config::SetValue(std::string section, std::string name, Hotkey value) 
{
	auto outString = string_format("%d %d", value.GetMKey(), value.GetAKey());
	m_INIFile.SetValue(section.c_str(), name.c_str(), outString.c_str());
}


void Config::LoadFieldValue(ToggleConfigField& field) {
	auto baseField = (ConfigField<bool>)field;
	LoadFieldValue(baseField);
	auto hotkeyField = field.GetHotkeyField();
	LoadFieldValue(hotkeyField);
}

// Config header
ConfigFieldHeader::ConfigFieldHeader(const std::string friendlyName, const std::string section, const std::string name, const uint32_t flags)
	: userName(friendlyName), section(section), name(name), flags(flags)
{}

std::string ConfigFieldHeader::GetFriendlyName() const
{
	return userName;
}

std::string ConfigFieldHeader::GetName() const
{
	return name;
}

std::string ConfigFieldHeader::GetSection() const
{
	return section;
}

uint32_t ConfigFieldHeader::GetFlags() const
{
	return flags;
}


// Toggle config field

ToggleConfigField::ToggleConfigField(const std::string friendlyName, const std::string section, const std::string name, const uint32_t flags, bool defaultValue, OnChangeCallback callback, OnChangeCallbackHotkey hotkeyCallback)
	: ToggleConfigField(friendlyName, section, name, flags, defaultValue, Hotkey(0, 0), callback, hotkeyCallback) { }

ToggleConfigField::ToggleConfigField(const std::string friendlyName, const std::string section, const std::string name, const uint32_t flags, bool defaultValue, Hotkey hotkey, OnChangeCallback callback, OnChangeCallbackHotkey hotkeyCallback)
	: ConfigField<bool>(friendlyName, section, name, flags | (uint32_t)ConfigFieldFlag::ToggleField, defaultValue, callback),
	hotkeyField(ConfigField<Hotkey>(friendlyName, "Hotkeys", name, flags, hotkey, hotkeyCallback)) { }

Hotkey* ToggleConfigField::GetHotkey()
{
	return hotkeyField.GetValuePtr();
}

ConfigField<Hotkey> ToggleConfigField::GetHotkeyField()
{
	return hotkeyField;
}

bool ToggleConfigField::Check()
{
	if (!ConfigField<bool>::Check())
		return false;

	OnChangedEvent(this);

	return true;
}
