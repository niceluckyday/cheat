#pragma once

#include "IEntityFilter.h"

namespace cheat::game
{
    class SimpleFilter :
        public IEntityFilter
    {
	public:

        SimpleFilter(app::EntityType__Enum_1 type, const std::vector<std::string>& names = {});
        bool IsValid(Entity* entity) const override;

    private:
		app::EntityType__Enum_1 m_Type;
        std::vector<std::string> m_Names;
    };
}
