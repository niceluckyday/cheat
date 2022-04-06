#pragma once
#include <cheat-base/config/FieldBase.h>

namespace config::field 
{
	template<class T>
	class BaseField : public FieldBase<T>
	{
	public:
		using base = FieldBase<T>;

		BaseField(const std::string friendlyName, const std::string name, const std::string section, T defaultValue)
			: base(friendlyName, name, section, defaultValue)
		{
		}

		virtual void Write(std::ostream& io) override
		{
			io << base::value();
		}

		virtual void Read(std::istream& io) override
		{
			auto& value = *base::valuePtr();
			io >> value;
			*base::prevValue = value;
		}
	};
}


