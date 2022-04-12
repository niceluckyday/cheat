#pragma once
#include <cheat/esp/IEntityFilter.h>
#include <cheat/game.h>

namespace cheat::feature::esp
{
    class SimpleFilter :
        public IEntityFilter
    {
	public:

        SimpleFilter(const game::SimpleEntityFilter& filter);
        bool IsValid(app::BaseEntity* entity) override;

    private:
        game::SimpleEntityFilter m_Filter;
    };
}
