#include "pch-il2cpp.h"
#include "ESPFilterEntry.h"

namespace cheat::feature::esp
{

	bool ESPFilterEntry::operator==(const ESPFilterEntry& another) const
	{
		return m_Enabled == another.m_Enabled && m_Color == another.m_Color && m_EnabledHotkey == another.m_EnabledHotkey;
	}
}