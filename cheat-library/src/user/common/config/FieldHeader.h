#pragma once
#include <string>
namespace config 
{
	class FieldHeader
	{
	public:
		FieldHeader(const std::string friendlyName, const std::string section, const std::string name);

		std::string GetFriendlyName() const;
		std::string GetName() const;
		std::string GetSection() const;

	private:
		std::string name;
		std::string section;
		std::string userName;
	};
}


