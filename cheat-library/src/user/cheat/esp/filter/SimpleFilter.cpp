#include "pch-il2cpp.h"
#include "SimpleFilter.h"

bool cheat::feature::esp::SimpleFilter::IsValid(app::BaseEntity* entity)
{
	return game::IsEntityFilterValid(entity, m_Filter);
}

cheat::feature::esp::SimpleFilter::SimpleFilter(const game::SimpleEntityFilter& filter) : m_Filter(filter)
{}
