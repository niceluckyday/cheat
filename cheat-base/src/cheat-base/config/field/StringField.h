#pragma once
#include "BaseField.h"
#include "HotkeyField.h"

namespace config::field 
{
	class StringField : public BaseField<std::string>
	{
	public:
		StringField(const std::string friendlyName, const std::string name, const std::string section, std::string defaultValue);

		virtual void Read(std::istream& io) override;
	};
}


