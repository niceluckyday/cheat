#include <pch-il2cpp.h>
#include "ESPFilterField.h"

#include <cheat-base/util.h>
#include <cheat-base/config/Config.h>

namespace config::field
{
	ESPFilterField::ESPFilterField(const std::string friendlyName, const std::string name, const std::string section) :
		FieldBase<cheat::feature::esp::ESPFilterEntry>(friendlyName, name, section,
			cheat::feature::esp::ESPFilterEntry(name, ImColor(120, 120, 120, 255), Hotkey(), name + ".png"))
	{
	}

	void ESPFilterField::Write(std::ostream& io)
	{
		io << value().m_Enabled << " ";

		io << (ImU32)value().m_Color;

		auto keys = value().m_EnabledHotkey.GetKeys();
		for (auto it = keys.begin(); it != keys.end(); it++)
		{
			io << " " << * it;
		}
	}

	void ESPFilterField::Read(std::istream& io)
	{
		io >> valuePtr()->m_Enabled;

		ImU32 color;
		io >> color;
		valuePtr()->m_Color = ImColor(color);

		std::vector<short> keys;

		short key;
		while (io >> key)
			keys.push_back(key);

		valuePtr()->m_EnabledHotkey = Hotkey(keys);
	}

}

