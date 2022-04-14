#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>

#include <cheat/game/IEntityFilter.h>

namespace cheat::feature 
{

	class ItemTeleportBase : public Feature, public game::IEntityFilter
    {
	public:
		config::field::HotkeyField m_Key;
		config::field::BaseField<bool> m_ShowInfo;

		void DrawMain() final;
		
		virtual void DrawItems();
		virtual void DrawFilterOptions() { };

		virtual bool NeedInfoDraw() const;
		virtual void DrawInfo() override;

		void OnKeyUp(short key, bool& cancelled);
	
	protected:
		ItemTeleportBase(const std::string& section, const std::string& name);
		
		void DrawEntityInfo(game::Entity* entity);

	private:
		void DrawEntities();
		void DrawNearestEntityInfo();
		
		std::string section;
		std::string name;
	};
}

