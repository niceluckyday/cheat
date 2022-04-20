#pragma once

#include "IEntityFilter.h"
#include <vector>
#include <il2cpp-appdata.h>

namespace cheat::game
{
    class AdvancedFilter :
        public IEntityFilter
    {
    public:
        AdvancedFilter(const std::vector<app::EntityType__Enum_1>& types = {}, const std::vector<std::string>& names = {});
        AdvancedFilter(const app::EntityType__Enum_1 type, const std::string name) : AdvancedFilter(std::vector<app::EntityType__Enum_1>{type}, std::vector<std::string>{name}) {}
        bool IsValid(Entity* entity) const override;
        friend AdvancedFilter operator+(AdvancedFilter lFilter, const AdvancedFilter& rFilter);

    private:
        std::vector<app::EntityType__Enum_1> m_Types;
        std::vector<std::string> m_Names;
    };
}
