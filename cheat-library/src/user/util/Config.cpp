#include "pch-il2cpp.h"
#include "Config.h"

#include <sstream>

#include <simple-ini.hpp>

#include <helpers.h>
#include <util/Logger.h>

static bool changed = false;

static std::string filename;

#define LoadToggleValue(field) LoadFieldValue(field); toggleFields.push_back(field)

void Config::Init(const std::string configFile)
{
	filename = configFile;

	auto status = m_INIFile.LoadFile(configFile.c_str());
	if (status < 0)
		LOG_ERROR("Failed to load config file.");

	LoadToggleValue(cfgGodModEnable);
	LoadToggleValue(cfgInfiniteStaminaEnable);
	LoadToggleValue(cfgInstantBowEnable);
	LoadToggleValue(cfgNoCDEnable);
	LoadToggleValue(cfgNoGravityEnable);
	LoadToggleValue(cfgMoveSpeedhackEnable);

	LoadToggleValue(cfgUnlockWaypointsEnable);
	LoadToggleValue(cfgDummyEnemiesEnabled);

	LoadToggleValue(cfgMapTPEnable);
	LoadFieldValue(cfgTeleportHeight);
	LoadFieldValue(cfgTeleportKey);
	LoadFieldValue(cfgUseOfflineTeleport);

	LoadFieldValue(cfgDisableMhyprot);
	LoadFieldValue(cfgConsoleLogEnabled);
	LoadFieldValue(cfgFileLogEnabled);
	LoadFieldValue(cfgMoveStatusWindow);
}

std::vector<ToggleConfigField> Config::GetToggleFields()
{
	return toggleFields;
}

void Config::Save()
{
	auto status = m_INIFile.SaveFile(filename.c_str());
	if (status < 0)
		LOG_ERROR("Failed to save changes to config.");
}

void Config::SetValue(std::string section, std::string name, char* value) {
	m_INIFile.SetValue(section.c_str(), name.c_str(), value);
}

void Config::SetValue(std::string section, std::string name, float value) {
	m_INIFile.SetDoubleValue(section.c_str(), name.c_str(), (double)value);
}

void Config::SetValue(std::string section, std::string name, bool value) {
	m_INIFile.SetBoolValue(section.c_str(), name.c_str(), value);
}

void Config::SetValue(std::string section, std::string name, int value) {
	m_INIFile.SetLongValue(section.c_str(), name.c_str(), (long)value);
}

void Config::SetValue(std::string section, std::string name, Hotkey value) {
	auto outString = string_format("%d %d", value.GetMKey(), value.GetAKey());
	m_INIFile.SetValue(section.c_str(), name.c_str(), outString.c_str());
}

void Config::LoadFieldValue(ConfigField<bool>& field) {
	field = m_INIFile.GetBoolValue(field.GetSection().c_str(), field.GetName().c_str(), field.GetValue());
}

void Config::LoadFieldValue(ConfigField<int>& field) {
	field = (int)m_INIFile.GetLongValue(field.GetSection().c_str(), field.GetName().c_str(), field.GetValue());
}

void Config::LoadFieldValue(ConfigField<float>& field) {
	field = (float)m_INIFile.GetDoubleValue(field.GetSection().c_str(), field.GetName().c_str(), field.GetValue());
}

void Config::LoadFieldValue(ConfigField<char*>& field) {
	field = (char*)m_INIFile.GetValue(field.GetSection().c_str(), field.GetName().c_str(), field.GetValue());
}

void Config::LoadFieldValue(ConfigField<Hotkey>& field) {
	const char* rawValue = m_INIFile.GetValue(field.GetSection().c_str(), field.GetName().c_str(), nullptr);
	if (rawValue == nullptr)
		return;

	std::stringstream stream;
	stream << rawValue;
	int mKey = 0, aKey = 0;
	stream >> mKey >> aKey;
	field = Hotkey(mKey, aKey);
}

void Config::LoadFieldValue(ToggleConfigField& field) {
	auto baseField = (ConfigField<bool>)field;
	LoadFieldValue(baseField);
	auto hotkeyField = field.GetHotkeyField();
	LoadFieldValue(hotkeyField);
}