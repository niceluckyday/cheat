#include "pch-il2cpp.h"
#include "AdvancedFilter.h"

namespace cheat::game
{

	bool AdvancedFilter::IsValid(Entity* entity) const 
	{
		if (entity == nullptr)
			return false;


		if (m_Types.size() == 0)
			return true;

		bool found = false;
		for (auto& type : m_Types)
		{
			if (entity->type() == type)
			{
				found = true;
				break;
			}					
		}if(found == false)
			return false;
		
		
		auto& name = entity->name();
		for (auto& pattern : m_Names)
		{
			if (name.find(pattern) != -1)
				return true;
		}
		

		return false;
	}

	AdvancedFilter::AdvancedFilter(const std::vector<app::EntityType__Enum_1>& types, const std::vector<std::string>& names ) : m_Names(names), m_Types(types)
	{

	}

	AdvancedFilter operator+(AdvancedFilter lFilter, const AdvancedFilter& rFilter)
	{
		lFilter.m_Names.insert(lFilter.m_Names.end(), rFilter.m_Names.begin(), rFilter.m_Names.end());
		return lFilter;
	}
	
	AdvancedFilter operator+(AdvancedFilter lFilter, const std::vector<app::EntityType__Enum_1>& rFilter)
	{
		lFilter.m_Types.insert(lFilter.m_Types.end(), rFilter.begin(), rFilter.end());
		return lFilter;
	}
	
}
