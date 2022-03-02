#include "pch-il2cpp.h"

#include <sstream>

#include "Config.h"

#include "simple-ini.hpp"
#include "helpers.h"
#include <util/Logger.h>


// Logging
ConfigField<bool> Config::cfgConsoleLogging("Logging", "ConsoleLog", true, Config::OnChangeValue);
ConfigField<bool> Config::cfgFileLogging("Logging", "FileLog", false, Config::OnChangeValue);

// Main
ConfigField<bool> Config::cfgDisableMhyProt("DLLUtil", "DisableMhyprot", true, Config::OnChangeValue);
ConfigField<Hotkey> Config::cfgMenuEnableKey("DLLUtil", "MenuShowKey", Hotkey(VK_F1, 0), Config::OnChangeValue);

// Teleportation
ConfigField<bool> Config::cfgTpToMarkEnable("Teleport", "Enable", true, Config::OnChangeValue);
ConfigField<float> Config::cfgTpHeight("Teleport", "TeleportHeight", 300.0f, Config::OnChangeValue);
ConfigField<Hotkey> Config::cfgTpToMarkKey("Teleport", "MarkTeleportationKey", Hotkey('T', 0), Config::OnChangeValue);
ConfigField<bool> Config::cfgUseOfflineTP("Teleport", "UseOfflineTeleport", false, Config::OnChangeValue);

CSimpleIni Config::m_INIFile;
static bool changed = false;

static std::string filename;

void Config::Init(const std::string configFile)
{
	filename = configFile;

	auto status = m_INIFile.LoadFile(configFile.c_str());
	if (status < 0)
		LOG_ERROR("Failed to load config file.");

	LoadFieldValue(cfgDisableMhyProt);
	LoadFieldValue(cfgConsoleLogging);

	LoadFieldValue(cfgTpHeight);
	LoadFieldValue(cfgTpToMarkEnable);
	LoadFieldValue(cfgTpToMarkKey);
	LoadFieldValue(cfgUseOfflineTP);
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