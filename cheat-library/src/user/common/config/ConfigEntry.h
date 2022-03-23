#pragma once
#include <common/Event.h>
#include <iostream>
#include "FieldHeader.h"

namespace config 
{
	class ConfigEntry : public FieldHeader
	{
	public:
		ConfigEntry(const std::string friendName, const std::string name, const std::string section) :
			FieldHeader(friendName, section, name) {}

		TEvent<ConfigEntry*> ChangedEvent;
		virtual void Write(std::ostream& io) = 0;
		virtual void Read(std::istream& io) = 0;
	};
}