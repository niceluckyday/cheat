#pragma once
#include "BaseField.h"
#include "HotkeyField.h"

namespace config::field 
{
	class ToggleField : public BaseField<bool>
	{
	public:
		ToggleField(const std::string friendlyName, const std::string name, const std::string section, bool defaultValue);

		ToggleField(const std::string friendlyName, const std::string name, const std::string section, bool defaultValue,
			Hotkey defaultHotkey);

		HotkeyField& GetHotkeyField();

	private:
		HotkeyField hotkeyField;

		void OnHotkeyChanged(ConfigEntry* entry);
	};
}


