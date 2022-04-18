#include "pch-il2cpp.h"
#include "RapidFire.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>
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
		
		auto& manager = game::EntityManager::instance();
		auto targetEntity = manager.entity(targetID);
		if (targetEntity == nullptr)
			return m_Multiplier;

		auto baseCombat = targetEntity->combat();
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

		auto& manager = game::EntityManager::instance();
		int countOfAttacks = m_Multiplier;
		if (m_OnePunch)
		{
			auto targetEntity = manager.entity(targetID);
			auto baseCombat = targetEntity->combat();
			if (baseCombat == nullptr)
				return 1;

			app::Formula_CalcAttackResult(targetEntity, combat->fields._combatProperty_k__BackingField,
				baseCombat->fields._combatProperty_k__BackingField,
				attackResult, manager.avatar()->raw(), targetEntity->raw(), nullptr);
			countOfAttacks = CalcCountToKill(attackResult->fields.damage, targetID);
		}
		return countOfAttacks;
	}

	bool IsAvatarOwner(game::Entity entity)
	{
		auto& manager = game::EntityManager::instance();
		auto avatarID = manager.avatar()->runtimeID();

		while (entity.isGadget())
		{
			game::Entity temp = entity;
			entity = game::Entity(app::GadgetEntity_GetOwnerEntity(reinterpret_cast<app::GadgetEntity*>(entity.raw()), nullptr));
			if (entity.runtimeID() == avatarID)
				return true;
		} 

		return false;
		
	}

	bool IsAttackByAvatar(game::Entity& attacker)
	{
		if (attacker.raw() == nullptr)
			return false;

		auto& manager = game::EntityManager::instance();
		auto avatarID = manager.avatar()->runtimeID();
		auto attackerID = attacker.runtimeID();

		return attackerID == avatarID || IsAvatarOwner(attacker);
	}

	// Raises when any entity do hit event.
	// Just recall attack few times (regulating by combatProp)
	// It's not tested well, so, I think, anticheat can detect it.
	static void LCBaseCombat_DoHitEntity_Hook(app::LCBaseCombat* __this, uint32_t targetID, app::AttackResult* attackResult,
		bool ignoreCheckCanBeHitInMP, MethodInfo* method)
	{
		//SAFE_BEGIN();
		
		auto attacker = game::Entity(__this->fields._._._entity);
		if (!IsAttackByAvatar(attacker))
			return callOrigin(LCBaseCombat_DoHitEntity_Hook, __this, targetID, attackResult, ignoreCheckCanBeHitInMP, method);

		RapidFire& rapidFire = RapidFire::GetInstance();
		int attackCount = rapidFire.GetAttackCount(__this, targetID, attackResult);
		for (int i = 0; i < attackCount; i++)
			callOrigin(LCBaseCombat_DoHitEntity_Hook, __this, targetID, attackResult, ignoreCheckCanBeHitInMP, method);
		
		//SAFE_ERROR();

		//callOrigin(LCBaseCombat_DoHitEntity_Hook, __this, targetID, attackResult, ignoreCheckCanBeHitInMP, method);
		//
		//SAFE_END();
	}
}

