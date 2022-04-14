#pragma once

#include <cheat-base/cheat/Feature.h>

#include <cheat/esp/data/ESPItemField.h>
#include <cheat/game/IEntityFilter.h>
#include <cheat/game/CacheFilterExecutor.h>

namespace cheat::feature::esp
{
	class ESPSectionBase : public cheat::Feature
	{

	public:
		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawExternal() override;
		void DrawMain() override;

	protected:
		ESPSectionBase(std::string filterName);
		void AddFilter(const std::string& name, game::IEntityFilter* filter);

	private:
		std::vector<std::pair<config::field::ESPItemField*, game::IEntityFilter*>> m_Filters;
		std::string m_Name;
		game::CacheFilterExecutor m_FilterExecutor;

		void DrawFilterField(config::field::ESPItemField& field);

		void OnKeyUp(short key, bool& cancelled);

	};
}


