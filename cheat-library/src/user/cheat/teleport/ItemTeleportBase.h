#pragma once
#include <cheat/Feature.h>
#include <common/config/Config.h>
#include <common/util.h>

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
		ItemTeleportBase(const std::string& section, const std::string& name, FilterFunc filter);
	
	private:
		void DrawEntities(const FilterFunc& filter);
		void DrawNearestEntityInfo(const char* prefix, FilterFunc filter);
		
		std::string section;
		std::string name;
		FilterFunc filter;
	};
}

