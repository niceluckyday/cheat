#include "pch-il2cpp.h"
#include "AutoOreDestroy.h"

#include <helpers.h>
#include <algorithm>

#include <cheat/events.h>
#include <cheat/game/SimpleFilter.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/filters.h>

namespace cheat::feature 
{
	static void LCAbilityElement_ReduceModifierDurability_Hook(app::LCAbilityElement* __this, int32_t modifierDurabilityIndex, float reduceDurability, app::Nullable_1_Single_ deltaTime, MethodInfo* method);

    AutoOreDestroy::AutoOreDestroy() : Feature(),
        NF(m_Enabled,			"Auto Destroy",  "AutoOreDestroy", false),
		NF(m_DestroyOres,		"Destroy Ores",			"AutoOreDestroy", false),
		NF(m_DestroyShields,	"Destroy Shields",		"AutoOreDestroy", false),
		NF(m_DestroyDoodads,	"Destroy Doodads",		"AutoOreDestroy", false),
        NF(m_Range,				"Range",				"AutoOreDestroy", 10.0f)
    { 
		HookManager::install(app::LCAbilityElement_ReduceModifierDurability, LCAbilityElement_ReduceModifierDurability_Hook);
	}

    const FeatureGUIInfo& AutoOreDestroy::GetGUIInfo() const
    {
        static const FeatureGUIInfo info { "Auto Destroy Objects", "World", true };
        return info;
    }

    void AutoOreDestroy::DrawMain()
    {
		ImGui::TextColored(ImColor(255, 165, 0, 255), "Note. This feature is not fully tested detection-wise.\n"
			"Not recommended for main accounts or used with high values.");
		
		ConfigWidget("Enabled", m_Enabled, "Instantly destroys non-living objects within range.");
		ImGui::Indent();
		ConfigWidget("Ores", m_DestroyOres, "Ores and variants, e.g. electro crystals, marrows, etc.");
		ConfigWidget("Shields", m_DestroyShields, "Abyss mage/churl/slime shields.");
		ConfigWidget("Doodads", m_DestroyDoodads, "Barrels, boxes, vases, etc.");
		ImGui::Unindent();
		ConfigWidget("Range (m)", m_Range, 0.1f, 1.0f, 15.0f);
    }

    bool AutoOreDestroy::NeedStatusDraw() const
	{
        return m_Enabled;
    }

    void AutoOreDestroy::DrawStatus() 
    { 
		ImGui::Text("Destroy [%.01fm%s%s%s%s]",
			m_Range.value(),
			m_DestroyOres || m_DestroyShields || m_DestroyDoodads ? "|" : "",
			m_DestroyOres ? "O" : "",
			m_DestroyShields ? "S" : "",
			m_DestroyDoodads ? "D" : "");
    }

    AutoOreDestroy& AutoOreDestroy::GetInstance()
    {
        static AutoOreDestroy instance;
        return instance;
    }

	// Thanks to @RyujinZX
	// Every ore has ability element component
	// Durability of ability element is a ore health
	// Every tick ability element check reducing durability, for ore in calm state `reduceDurability` equals 0, means HP don't change
	// We need just change this value to current durability or above to destroy ore
	// This function also can work with some types of shields (TODO: improve killaura with this function)
	static void LCAbilityElement_ReduceModifierDurability_Hook(app::LCAbilityElement* __this, int32_t modifierDurabilityIndex, float reduceDurability, app::Nullable_1_Single_ deltaTime, MethodInfo* method)
	{
		auto& manager = game::EntityManager::instance();
		auto& autoDestroy = AutoOreDestroy::GetInstance();
		auto entity = __this->fields._._._entity;
		if (autoDestroy.m_Enabled && 
			autoDestroy.m_Range > manager.avatar()->distance(entity) &&
			(
				(autoDestroy.m_DestroyOres && game::filters::combined::Ores.IsValid(manager.entity(entity))) || 
				(autoDestroy.m_DestroyDoodads && game::filters::combined::Doodads.IsValid(manager.entity(entity))) ||
				(autoDestroy.m_DestroyShields && game::filters::combined::Living.IsValid(manager.entity(entity)))
			)
		)
		{
			// This value always above any ore durability
			reduceDurability = 1000;
		}
		callOrigin(LCAbilityElement_ReduceModifierDurability_Hook, __this, modifierDurabilityIndex, reduceDurability, deltaTime, method);
	}

}

