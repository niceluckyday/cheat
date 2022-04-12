#include <pch.h>
#include "StringField.h"

#include <cheat-base/util.h>
#include <cheat-base/config/Config.h>

namespace config::field
{

	StringField::StringField(const std::string friendlyName, const std::string name, const std::string section, std::string defaultValue) :
		BaseField<std::string>(friendlyName, name, section, defaultValue)
	{ }

	void StringField::Read(std::istream& io)
	{
		*valuePtr() = std::string(std::istreambuf_iterator<char>(io), {});
	}
}

