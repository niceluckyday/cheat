#include <pch.h>
#include "HotkeyField.h"

void config::field::HotkeyField::Write(std::ostream& io)
{
	auto keys = value().GetKeys();
	for (auto it = keys.begin(); it != keys.end(); it++)
	{
		if (it != keys.begin())
			io << " ";
		io << *it;
	}
		
}

void config::field::HotkeyField::Read(std::istream& io)
{
	size_t keysSize = 0;
	std::vector<short> keys;

	short key;
	while (io >> key)
	{
		LOG_DEBUG("Key: %d", key);
		keys.push_back(key);
	}

	*valuePtr() = Hotkey(keys);
	*prevValue = value();
}
