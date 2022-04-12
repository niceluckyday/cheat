#pragma once
namespace cheat::feature::esp
{
	class IEntityFilter
	{
	public:
		virtual bool IsValid(app::BaseEntity* entity) = 0;
	};
}

