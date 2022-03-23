#include <pch.h>
#include "FieldHeader.h"

config::FieldHeader::FieldHeader(const std::string friendlyName, const std::string section, const std::string name) 
	: userName(friendlyName), section(section), name(name)
{
}

std::string config::FieldHeader::GetFriendlyName() const
{
	return userName;
}

std::string config::FieldHeader::GetName() const
{
	return name;
}

std::string config::FieldHeader::GetSection() const
{
	return section;
}
