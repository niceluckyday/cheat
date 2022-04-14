#pragma once
#include "cheat-base/config/field/BaseField.h"
#include <cheat/esp/data/ESPItem.h>

namespace config::field 
{
	class ESPItemField : public FieldBase<cheat::feature::esp::ESPItem>
	{
	public:

		ESPItemField(const std::string friendlyName, const std::string name, const std::string section);

		void Write(std::ostream& io) override;

		void Read(std::istream& io) override;

	};
}


