#pragma once
#include "BaseField.h"
#include <cheat-base/Hotkey.h>

namespace config::field 
{
	class HotkeyField : public FieldBase<Hotkey>
	{
	public:
		HotkeyField(const std::string friendlyName, const std::string name, const std::string section, Hotkey defaultValue)
			: FieldBase<Hotkey>(friendlyName, name, section, defaultValue)
		{
		}
		virtual void Write(std::ostream& io) override;
		virtual void Read(std::istream& io) override;
	};
}


