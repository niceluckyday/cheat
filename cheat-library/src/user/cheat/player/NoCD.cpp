#include "pch-il2cpp.h"
#include "NoCD.h"

#include <helpers.h>
#include <fmt/chrono.h>

namespace cheat::feature 
{
	static bool HumanoidMoveFSM_CheckSprintCooldown_Hook(void* __this, MethodInfo* method);
	static bool LCAvatarCombat_IsEnergyMax_Hook(void* __this, MethodInfo* method);
	static bool LCAvatarCombat_IsSkillInCD_1_Hook(void* __this, void* skillInfo, MethodInfo* method);
	static void ActorAbilityPlugin_AddDynamicFloatWithRange_Hook(void* __this, app::String* key, float value, float minValue, float maxValue,
		bool forceDoAtRemote, MethodInfo* method);

	static std::list<std::string> abilityLog;

    NoCD::NoCD() : Feature(),
        NF(m_Ability,    "Ability CD",  "NoCD", false),
        NF(m_Sprint,     "Sprint CD",   "NoCD", false),
        NF(m_InstantBow, "Instant bow", "NoCD", false)
    {
		HookManager::install(app::LCAvatarCombat_IsEnergyMax, LCAvatarCombat_IsEnergyMax_Hook);
		HookManager::install(app::LCAvatarCombat_IsSkillInCD_1, LCAvatarCombat_IsSkillInCD_1_Hook);

		HookManager::install(app::HumanoidMoveFSM_CheckSprintCooldown, HumanoidMoveFSM_CheckSprintCooldown_Hook);

		HookManager::install(app::ActorAbilityPlugin_AddDynamicFloatWithRange, ActorAbilityPlugin_AddDynamicFloatWithRange_Hook);
    }

    const FeatureGUIInfo& NoCD::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "No cooldown", "Player", true };
        return info;
    }

    void NoCD::DrawMain()
    {
		ConfigWidget(m_InstantBow, "Disable cooldown of bow charge.");
		ConfigWidget(m_Ability, "Disable skills cooldown.");
		ConfigWidget(m_Sprint, "Disable use sprint delay.");

		ImGui::Text("If instant bow charge doesn't work:");
		TextURL("Check issue on github.", "https://github.com/CallowBlack/genshin-cheat/issues/47", false, false);
		if (ImGui::TreeNode("Ability log [DEBUG]"))
		{
			if (ImGui::Button("Copy to clipboard"))
			{
				ImGui::LogToClipboard();

				ImGui::LogText("Ability log:\n");

				for (auto& logEntry : abilityLog)
					ImGui::LogText("%s\n", logEntry.c_str());

				ImGui::LogFinish();
			}

			for (std::string& logEntry : abilityLog)
				ImGui::Text(logEntry.c_str());

			ImGui::TreePop();
		}
    }

    bool NoCD::NeedStatusDraw() const
{
        return m_InstantBow || m_Ability;
    }

    void NoCD::DrawStatus() 
    {
        if (m_InstantBow)
            ImGui::Text("Instant bow");

        if (m_Ability)
            ImGui::Text("No ability CD");
    }

    NoCD& NoCD::GetInstance()
    {
        static NoCD instance;
        return instance;
    }

	static bool LCAvatarCombat_IsEnergyMax_Hook(void* __this, MethodInfo* method)
	{
		NoCD& noCD = NoCD::GetInstance();
		if (noCD.m_Ability)
			return true;

		return callOrigin(LCAvatarCombat_IsEnergyMax_Hook, __this, method);
	}

	static bool LCAvatarCombat_IsSkillInCD_1_Hook(void* __this, void* skillInfo, MethodInfo* method)
	{
		NoCD& noCD = NoCD::GetInstance();
		if (noCD.m_Ability)
			return false;

		return callOrigin(LCAvatarCombat_IsSkillInCD_1_Hook, __this, skillInfo, method);
	}

	// Check sprint cooldown, we just return true if sprint no cooldown enabled.
	static bool HumanoidMoveFSM_CheckSprintCooldown_Hook(void* __this, MethodInfo* method)
	{
		NoCD& noCD = NoCD::GetInstance();
		if (noCD.m_Sprint)
			return true;

		return callOrigin(HumanoidMoveFSM_CheckSprintCooldown_Hook, __this, method);
	}

	// This function raise when abilities, whose has charge, is charging, like a bow.
	// value - increase value
	// min and max - bounds of charge.
	// So, to charge make full charge m_Instantly, just replace value to maxValue.
	static void ActorAbilityPlugin_AddDynamicFloatWithRange_Hook(void* __this, app::String* key, float value, float minValue, float maxValue,
		bool forceDoAtRemote, MethodInfo* method)
	{
		std::time_t t = std::time(nullptr);
		auto logEntry = fmt::format("{:%H:%M:%S} | Key: {} value {}.", fmt::localtime(t), il2cppi_to_string(key), value);
		abilityLog.push_front(logEntry);
		if (abilityLog.size() > 50)
			abilityLog.pop_back();

		NoCD& noCD = NoCD::GetInstance();
		// This function is calling not only for bows, so if don't put key filter it cause various game mechanic bugs.
		// For now only "_Enchanted_Time" found for bow charging, maybe there are more. Need to continue research.
		if (noCD.m_InstantBow && il2cppi_to_string(key) == "_Enchanted_Time")
			value = maxValue;
		callOrigin(ActorAbilityPlugin_AddDynamicFloatWithRange_Hook, __this, key, value, minValue, maxValue, forceDoAtRemote, method);
	}
}

