#include "pch-il2cpp.h"
#include "RapidFire.h"

#include <helpers.h>
#include <cheat/game.h>

namespace cheat::feature 
{
	static void LCBaseCombat_DoHitEntity_Hook(app::LCBaseCombat* __this, uint32_t targetID, app::AttackResult* attackResult,
		bool ignoreCheckCanBeHitInMP, MethodInfo* method);

    RapidFire::RapidFire() : Feature(),
        NF(m_Enabled,    "Rapid fire",     "RapidFire", false),
        NF(m_Multiplier, "Multiplier",     "RapidFire", 2),
        NF(m_OnePunch,   "One punch mode", "RapidFire", false)
    {
		HookManager::install(app::LCBaseCombat_DoHitEntity, LCBaseCombat_DoHitEntity_Hook);
    }

    const FeatureGUIInfo& RapidFire::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "Rapid fire", "Player", true };
        return info;
    }

    void RapidFire::DrawMain()
    {
		ConfigWidget("Enabled", m_Enabled, "Enables rapid fire.\n\
            Rapid fire just multiply your attack count.\n\
            It's not well tested, and can be detected by anticheat.\n\
            So not recommend to you that in your main account.");

		ConfigWidget(m_Multiplier, 1, 2, 1000, "Rapid fire multiply count.\n\
            Each hit is multiplied by the multiplier.\n\
            Note that large values may cause some lag.");

		ConfigWidget(m_OnePunch, "Calculate how many attacks needed to kill an enemy.\n\
            Since the damage is calculated from the server, The calculation may not be on-point.");
    }

    bool RapidFire::NeedStatusDraw() const
{
        return m_Enabled;
    }

    void RapidFire::DrawStatus() 
    {
        if (m_OnePunch)
            ImGui::Text("Rapid fire [OnePunch]");
        else
            ImGui::Text("Rapid fire [%d]", m_Multiplier.value());
    }

    RapidFire& RapidFire::GetInstance()
    {
        static RapidFire instance;
        return instance;
    }


	int RapidFire::CalcCountToKill(float attackDamage, uint32_t targetID)
	{
		if (attackDamage == 0)
			return m_Multiplier;

		auto targetEntity = game::GetEntityByRuntimeId(targetID);
		if (targetEntity == nullptr)
			return m_Multiplier;

		auto baseCombat = app::BaseEntity_GetBaseCombat(targetEntity, *app::BaseEntity_GetBaseCombat__MethodInfo);
		if (baseCombat == nullptr)
			return m_Multiplier;

		auto safeHP = baseCombat->fields._combatProperty_k__BackingField->fields.HP;
		auto HP = app::SafeFloat_GetValue(nullptr, safeHP, nullptr);
		int attackCount = (int)ceil(HP / attackDamage);
		return std::clamp(attackCount, 1, 1000);
	}

	int RapidFire::GetAttackCount(app::LCBaseCombat* combat, uint32_t targetID, app::AttackResult* attackResult)
	{
		if (!m_Enabled)
			return 1;

		int countOfAttacks = m_Multiplier;
		if (m_OnePunch)
		{
			auto targetEntity = game::GetEntityByRuntimeId(targetID);
			auto baseCombat = app::BaseEntity_GetBaseCombat(targetEntity, *app::BaseEntity_GetBaseCombat__MethodInfo);
			app::Formula_CalcAttackResult(targetEntity, combat->fields._combatProperty_k__BackingField,
				baseCombat->fields._combatProperty_k__BackingField,
				attackResult, game::GetAvatarEntity(), targetEntity, nullptr);
			countOfAttacks = CalcCountToKill(attackResult->fields.damage, targetID);
		}
		return countOfAttacks;
	}

	// Raises when any entity do hit event.
	// Just recall attack few times (regulating by combatProp)
	// It's not tested well, so, I think, anticheat can detect it.
	static void LCBaseCombat_DoHitEntity_Hook(app::LCBaseCombat* __this, uint32_t targetID, app::AttackResult* attackResult,
		bool ignoreCheckCanBeHitInMP, MethodInfo* method)
	{
		auto avatarID = game::GetAvatarRuntimeId();
		auto attackerID = __this->fields._._.entityRuntimeID;
		auto gadget = game::GetGadget(attackerID);
		if (attackerID != avatarID && (gadget == nullptr || gadget->fields._ownerRuntimeID != avatarID))
			return callOrigin(LCBaseCombat_DoHitEntity_Hook, __this, targetID, attackResult, ignoreCheckCanBeHitInMP, method);

		RapidFire& rapidFire = RapidFire::GetInstance();
		int attackCount = rapidFire.GetAttackCount(__this, targetID, attackResult);
		for (int i = 0; i < attackCount; i++)
			callOrigin(LCBaseCombat_DoHitEntity_Hook, __this, targetID, attackResult, ignoreCheckCanBeHitInMP, method);
	}
}

