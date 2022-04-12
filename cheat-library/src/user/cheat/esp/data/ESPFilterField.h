#pragma once
#include "cheat-base/config/field/BaseField.h"
#include <cheat/esp/data/ESPFilterEntry.h>

namespace config::field 
{
	class ESPFilterField : public FieldBase<cheat::feature::esp::ESPFilterEntry>
	{
	public:

		ESPFilterField(const std::string friendlyName, const std::string name, const std::string section);

		void Write(std::ostream& io) override;

		void Read(std::istream& io) override;

	};
}


