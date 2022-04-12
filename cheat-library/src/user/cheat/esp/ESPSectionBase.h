#pragma once

#include <cheat-base/cheat/Feature.h>

#include <cheat/esp/data/ESPFilterField.h>
#include <cheat/esp/IEntityFilter.h>

#include <cheat/game.h>

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
		void AddFilter(const std::string& name, IEntityFilter* filter);

	private:
		std::vector<std::pair<config::field::ESPFilterField*, IEntityFilter*>> m_Filters;
		std::string m_Name;

		void DrawFilterField(config::field::ESPFilterField& field);

		void OnKeyUp(short key, bool& cancelled);

		void DrawBox(app::BaseEntity* entity, const ImColor& color);
		void DrawRect(app::BaseEntity* entity, const ImColor& color);
		void DrawLine(app::BaseEntity* entity, const ImColor& color);
	};
}


