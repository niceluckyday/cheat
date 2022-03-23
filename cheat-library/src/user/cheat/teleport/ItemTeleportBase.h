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

		virtual void DrawMain() final;

		virtual void DrawFilterOptions() { };

		virtual bool NeedStatusDraw() const { return false; };
		virtual void DrawStatus() final {};

		virtual bool NeedInfoDraw() const;
		virtual void DrawInfo() final;

		void OnKeyUp(short key, bool& cancelled);
	
	protected:
		ItemTeleportBase(const std::string& section, const std::string& name, game::FilterFunc filter);
	
	private:
		void DrawEntities(const game::FilterFunc& filter);
		void DrawNearestEntityInfo(const char* prefix, game::FilterFunc filter);
		
		std::string section;
		std::string name;
		game::FilterFunc filter;
	};
}

