#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>

#include <cheat/game.h>

namespace cheat::feature 
{

	class ItemTeleportBase : public Feature
    {
	public:
		config::field::HotkeyField m_Key;
		config::field::BaseField<bool> m_ShowInfo;

		void DrawMain() final;
		
		virtual void DrawItems();
		virtual void DrawFilterOptions() { };

		bool NeedStatusDraw() const final { return false; };
		void DrawStatus() final {};

		virtual bool NeedInfoDraw() const;
		virtual void DrawInfo() override;

		void OnKeyUp(short key, bool& cancelled);
	
	protected:
		ItemTeleportBase(const std::string& section, const std::string& name, game::FilterFunc filter);
		
		void DrawEntityInfo(app::BaseEntity* entity);

	private:
		void DrawEntities();
		void DrawNearestEntityInfo();
		
		std::string section;
		std::string name;
		game::FilterFunc filter;
	};
}

