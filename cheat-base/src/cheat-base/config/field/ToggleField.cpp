#include <pch.h>
#include "ToggleField.h"

#include <cheat-base/util.h>
#include <cheat-base/config/Config.h>

namespace config::field
{
	ToggleField::ToggleField(const std::string friendlyName, const std::string name, const std::string section, bool defaultValue)
		: ToggleField(friendlyName, name, section, defaultValue, Hotkey()) { }

	ToggleField::ToggleField(const std::string friendlyName, const std::string name, const std::string section, bool defaultValue, 
		Hotkey defaultHotkey) :
		BaseField<bool>(friendlyName, name, section, defaultValue), 
		NFF(hotkeyField, friendlyName, util::string_format("%s_%s", name, section), "Hotkey", defaultHotkey)
	{
		hotkeyField.ChangedEvent += MY_METHOD_HANDLER(ToggleField::OnHotkeyChanged);
	}

	void ToggleField::OnHotkeyChanged(ConfigEntry* entry)
	{
		ChangedEvent(entry);
	}

	HotkeyField& ToggleField::GetHotkeyField()
	{
		return hotkeyField;
	}
}

