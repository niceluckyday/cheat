#include "pch-il2cpp.h"
#include "AdvancedFilter.h"

namespace cheat::game
{
	AdvancedFilter::AdvancedFilter(const std::vector<app::EntityType__Enum_1>& types, const std::vector<std::string>& names)
		: m_Names(names), m_Types(types) { }

	bool AdvancedFilter::IsValid(Entity* entity) const 
	{
		if (entity == nullptr)
			return false;

		bool type_found = m_Types.size() == 0 || std::any_of(m_Types.cbegin(), m_Types.cend(),
			[entity](const app::EntityType__Enum_1& type) { return entity->type() == type; });
		
		if (!type_found)
			return false;

		auto& name = entity->name();
		bool name_found = m_Names.size() == 0 || std::any_of(m_Names.cbegin(), m_Names.cend(),
			[name](const std::string& pattern) { return name.find(pattern) != std::string::npos; });
		
		return name_found;
	}

	AdvancedFilter operator+(AdvancedFilter lFilter, const AdvancedFilter& rFilter)
	{
		lFilter.m_Names.insert(lFilter.m_Names.end(), rFilter.m_Names.begin(), rFilter.m_Names.end());
		lFilter.m_Types.insert(lFilter.m_Types.end(), rFilter.m_Types.begin(), rFilter.m_Types.end());
		return lFilter;
	}
}
