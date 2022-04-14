#pragma once
#include <cheat-base/config/FieldBase.h>
#include <imgui.h>

namespace config::field 
{
	class ColorField : public FieldBase<ImColor>
	{
	private:
		using base = FieldBase<ImColor>;

	public:
		ColorField(const std::string friendlyName, const std::string name, const std::string section, ImColor defaultValue)
			: base(friendlyName, name, section, defaultValue)
		{
		}

		void Write(std::ostream& io) override;
		void Read(std::istream& io) override;
	};
}


