#include <pch.h>
#include "Config.h"

namespace config 
{
	static bool changed = false;
	static std::string filename;
	static CSimpleIni ini;

	static std::vector<ConfigEntry*> fields{};
	static std::vector<field::ToggleField*> toggleFields{};
	static void OnFieldChanged(ConfigEntry* entry);

	void Init(const std::string configFile)
	{
		filename = configFile;

		auto status = ini.LoadFile(configFile.c_str());
		if (status < 0)
			LOG_ERROR("Failed to load config file.");

	}

	void Save()
	{
		auto status = ini.SaveFile(filename.c_str());
		if (status < 0)
			LOG_ERROR("Failed to save changes to config.");
	}

	std::vector<ConfigEntry*> GetFields()
	{
		return fields;
	}

	std::vector<field::ToggleField*> GetToggleFields()
	{
		return toggleFields;
	}

	void AddField(field::ToggleField& entry)
	{
		toggleFields.push_back(&entry);
		AddField((ConfigEntry&)entry);
	}

	void LoadField(ConfigEntry& field)
	{
		char* value = (char*)ini.GetValue(field.GetSection().c_str(), field.GetName().c_str());
		if (value == nullptr)
			return;

		std::stringstream stream(value);
		field.Read(stream);
	}

	void SetValue(ConfigEntry& field)
	{
		std::stringstream stream;
		field.Write(stream);
		ini.SetValue(field.GetSection().c_str(), field.GetName().c_str(), stream.str().c_str());
	}

	void AddField(ConfigEntry& entry)
	{
		LoadField(entry);
		entry.ChangedEvent += FREE_METHOD_HANDLER(OnFieldChanged);
		fields.push_back(&entry);
	}

	void OnFieldChanged(ConfigEntry* entry)
	{
		SetValue(*entry);
		Save();
	}
}

