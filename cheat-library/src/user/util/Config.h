#pragma once
#include <string>
#include <simple-ini.hpp>

#include <util/Hotkey.h>

template<class FieldType>
class ConfigField {
public:
	using OnChangeCallback = void (*)(ConfigField<FieldType>* field);

	void operator=(const FieldType& other) {
		*field = other;
		*prevValue = other;
		callback(this);
	}

	ConfigField(const std::string section, const std::string name, FieldType defaultValue, OnChangeCallback callback)
		: section(section), name(name), field(new FieldType(defaultValue)), prevValue(new FieldType(defaultValue)), callback(callback)
	{ }

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
		if (*prevValue == *field)
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
};

class Config {
public:

	// Is need to disable mhyprot2.sys protection.
	static ConfigField<bool> cfgDisableMhyProt;
	static ConfigField<Hotkey> cfgMenuEnableKey;

	// Logging
	static ConfigField<bool> cfgConsoleLogging;
	static ConfigField<bool> cfgFileLogging;

	// Teleportation
	static ConfigField<bool> cfgTpToMarkEnable;
	static ConfigField<float> cfgTpHeight;
	static ConfigField<Hotkey> cfgTpToMarkKey;
	static ConfigField<bool> cfgUseOfflineTP;

	static void Init(const std::string configFile);

	static void Save();

private:

	static CSimpleIni m_INIFile;

	static void SetValue(std::string section, std::string name, char* value);
	static void SetValue(std::string section, std::string name, float value);
	static void SetValue(std::string section, std::string name, bool value);
	static void SetValue(std::string section, std::string name, int value);
	static void SetValue(std::string section, std::string name, Hotkey value);

	static void LoadFieldValue(ConfigField<bool>& field);
	static void LoadFieldValue(ConfigField<int>& field);
	static void LoadFieldValue(ConfigField<float>& field);
	static void LoadFieldValue(ConfigField<char*>& field);
	static void LoadFieldValue(ConfigField<Hotkey>& field);

	template<class FieldType>
	static void OnChangeValue(ConfigField<FieldType>* field) {
		SetValue(field->GetSection(), field->GetName(), field->GetValue());
		Save();
	}
};