#pragma once
#include <cheat-base/config/FieldBase.h>
#include <magic_enum.hpp>

namespace config::field 
{
	template<class T>
	class EnumField : public FieldBase<T>
	{
	public:
		using base = FieldBase<T>;

		EnumField(const std::string friendlyName, const std::string name, const std::string section, T defaultValue)
			: base(friendlyName, name, section, defaultValue)
		{
		}

		virtual void Write(std::ostream& io) override
		{
			io << (size_t)base::value();
		}

		virtual void Read(std::istream& io) override
		{
			auto& value = *base::valuePtr();

			size_t numValue;
			io >> numValue;

			value = (T)numValue;
		}
	};
}


