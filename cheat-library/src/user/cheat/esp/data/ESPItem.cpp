#include "pch-il2cpp.h"
#include "ESPItem.h"

namespace cheat::feature::esp
{

	bool ESPItem::operator==(const ESPItem& another) const
	{
		return m_Enabled == another.m_Enabled && m_Color == another.m_Color && m_EnabledHotkey == another.m_EnabledHotkey;
	}
}