#include "pch-il2cpp.h"
#include "AutoOreDestroy.h"

#include <helpers.h>
#include <algorithm>

#include <cheat/events.h>
#include <cheat/game.h>

namespace cheat::feature 
{
	static void LCAbilityElement_ReduceModifierDurability_Hook(app::LCAbilityElement* __this, int32_t modifierDurabilityIndex, float reduceDurability, app::Nullable_1_Single_ deltaTime, MethodInfo* method);

    AutoOreDestroy::AutoOreDestroy() : Feature(),
        NF(m_Enabled,      "Auto ore destroy",  "AutoOreDestroy", false),
        NF(m_Range,        "Range",             "AutoOreDestroy", 10.0f)
    { 
		HookManager::install(app::LCAbilityElement_ReduceModifierDurability, LCAbilityElement_ReduceModifierDurability_Hook);
	}

    const FeatureGUIInfo& AutoOreDestroy::GetGUIInfo() const
    {
        static const FeatureGUIInfo info { "Auto ore destroy", "World", true };
        return info;
    }

    void AutoOreDestroy::DrawMain()
    {
		ImGui::TextColored(ImColor(255, 165, 0, 255), "Note. This feature not tested in detectable aspect.\n"
			"\tDon't recommend use it in main account in 5-10 days after release.");
		
		ConfigWidget("Enabled", m_Enabled, "Instantly destroys ores in specified range.");
		ConfigWidget(m_Range, 0.1f, 1.0f, 15.0f);
    }

    bool AutoOreDestroy::NeedStatusDraw() const
	{
        return m_Enabled;
    }

    void AutoOreDestroy::DrawStatus() 
    { 
        ImGui::Text("Ore destroy [%0.1fm]", m_Range.value());
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
		// Ore filter
		// Callow: I don't found any different way to find ores
		static const game::SimpleEntityFilter oreFilter = {
			{true, app::EntityType__Enum_1::GatherObject},
			{true, {
				"Crystalizedmarrow",
				"Thundercrystal",
				"OreNightBerth",
				"OreCrystal",
				"ElementRock",
				"OreMetal",
				"OreMoonMeteor",
				"OreElectricRock",
				"OreStone",
				"AncientOre"
				}
			}
		};

		auto& autoOreDestroy = AutoOreDestroy::GetInstance();
		auto entity = __this->fields._._._entity;
		if (autoOreDestroy.m_Enabled && 
			autoOreDestroy.m_Range > game::GetDistToAvatar(entity) && 
			game::IsEntityFilterValid(entity, oreFilter))
		{
			// This value always above any ore durability
			reduceDurability = 1000;
		}
		callOrigin(LCAbilityElement_ReduceModifierDurability_Hook, __this, modifierDurabilityIndex, reduceDurability, deltaTime, method);
	}

}

