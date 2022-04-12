#pragma once

#include <imgui.h>

#include <cheat-base/config/field/ToggleField.h>

namespace cheat::feature::esp
{
	class ESPFilterEntry
	{
	public:
		ESPFilterEntry(const std::string& name, const ImColor& color, const Hotkey& hotkey, const std::string& textureName)
			: m_Name(name), m_Color(color), m_EnabledHotkey(hotkey), m_Texture(nullptr), m_Enabled(false) { }

		bool operator==(const ESPFilterEntry& another) const;

		Hotkey m_EnabledHotkey;
		bool m_Enabled;
		std::string m_Name;
		ImColor m_Color;
		void* m_Texture;
	};
}


