#include "pch-il2cpp.h"
#include "DumbEnemies.h"

#include <helpers.h>

namespace cheat::feature 
{
    static void VCMonsterAIController_TryDoSkill_Hook(void* __this, uint32_t skillID, MethodInfo* method);

    DumbEnemies::DumbEnemies() : Feature(),
        NFF(m_Enabled, "Dumb enemies", "m_DumbEnemiesEnabled", "World", false)
    {
		HookManager::install(app::VCMonsterAIController_TryDoSkill, VCMonsterAIController_TryDoSkill_Hook);
    }

    const FeatureGUIInfo& DumbEnemies::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", "World", false };
        return info;
    }

    void DumbEnemies::DrawMain()
    {
        ConfigWidget(m_Enabled, "Enemies don't react to player.");
    }

    bool DumbEnemies::NeedStatusDraw() const
{
        return m_Enabled;
    }

    void DumbEnemies::DrawStatus() 
    { 
        ImGui::Text("Dumb enemies");
    }

    DumbEnemies& DumbEnemies::GetInstance()
    {
        static DumbEnemies instance;
        return instance;
    }

	// Raised when monster trying to do skill. Attack also is skill.
    // We just block if dumb mob enabled, so mob will not attack player.
	static void VCMonsterAIController_TryDoSkill_Hook(void* __this, uint32_t skillID, MethodInfo* method)
	{
        DumbEnemies& dumbEnemies = DumbEnemies::GetInstance();
		if (dumbEnemies.m_Enabled)
			return;
		callOrigin(VCMonsterAIController_TryDoSkill_Hook, __this, skillID, method);
	}
}

