#include "pch-il2cpp.h"
#include "KillAura.h"

#include <imgui.h>
#include <common/util.h>
#include <helpers.h>
#include <gui/gui-util.h>
#include <random>
#include <queue>
#include <unordered_set>

#include <cheat/events.h>

namespace cheat::feature 
{
    KillAura::KillAura() : Feature(),
        NF(m_Enabled,      "Kill aura",                 "KillAura", false),
        NF(m_OnlyTargeted, "Only targeted",             "KillAura", true),
        NF(m_Range,        "Range",                     "KillAura", 15.0f),
        NF(m_AttackDelay,  "Attack delay time (in ms)", "KillAura", 100),
        NF(m_RepeatDelay,  "Repeat delay time (in ms)", "KillAura", 1000)
    { 

		events::GameUpdateEvent += MY_METHOD_HANDLER(KillAura::OnGameUpdate);
	}

    const FeatureGUIInfo& KillAura::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "Kill aura", "World", true };
        return info;
    }

    void KillAura::DrawMain()
    {
		ConfigWidget("Enabled", m_Enabled, "Enables kill aura.\n" \
            "Kill aura cause crash damage for monster around you.");
		ConfigWidget(m_Range, 0.1f, 5.0f, 100.0f);
		ConfigWidget(m_OnlyTargeted, "If enabled, only monsters whose target at you will be affected by kill aura.");
		ConfigWidget(m_AttackDelay, 1, 0, 1000, "Delay before attack next monster.");
		ConfigWidget(m_RepeatDelay, 1, 100, 2000, "Delay before next attack same monster.");
    }

    bool KillAura::NeedStatusDraw() const
{
        return m_Enabled;
    }

    void KillAura::DrawStatus() 
    { 
        ImGui::Text("Kill aura [%.01fm]", m_Range.value());
    }

    KillAura& KillAura::GetInstance()
    {
        static KillAura instance;
        return instance;
    }

	void KillAura::OnGameUpdate()
	{
		static std::default_random_engine generator;
		static std::uniform_int_distribution<int> distribution(-50, 50);

		static int64_t nextAttackTime = 0;
		static std::map<uint32_t, int64_t> monsterRepeatTimeMap;
		static std::queue<app::BaseEntity*> attackQueue;
		static std::unordered_set<uint32_t> attackSet;

		if (!m_Enabled)
			return;

		auto eventManager = GetSingleton(EventManager);
		if (eventManager == nullptr || *app::CreateCrashEvent__MethodInfo == nullptr)
			return;

		auto currentTime = GetCurrentTimeMillisec();
		if (currentTime < nextAttackTime)
			return;

		for (const auto& monster : FindEntities(GetMonsterFilter()))
		{
			auto monsterID = monster->fields._runtimeID_k__BackingField;

			if (attackSet.count(monsterID) > 0)
				continue;

			if (monsterRepeatTimeMap.count(monsterID) > 0 && monsterRepeatTimeMap[monsterID] > currentTime)
				continue;

			auto combat = app::BaseEntity_GetBaseCombat(monster, *app::BaseEntity_GetBaseCombat__MethodInfo);
			if (combat == nullptr)
				continue;

			auto combatProp = combat->fields._combatProperty_k__BackingField;
			if (combatProp == nullptr)
				continue;

			auto maxHP = app::SafeFloat_GetValue(nullptr, combatProp->fields.maxHP, nullptr);
			auto isLockHp = app::FixedBoolStack_get_value(combatProp->fields.islockHP, nullptr);
			auto isInvincible = app::FixedBoolStack_get_value(combatProp->fields.isInvincible, nullptr);
			auto HP = app::SafeFloat_GetValue(nullptr, combatProp->fields.HP, nullptr);
			if (maxHP < 10 || HP < 2 || isLockHp || isInvincible)
				continue;

			if (m_OnlyTargeted && combat->fields._attackTarget.runtimeID != GetAvatarRuntimeId())
				continue;

			if (GetDistToAvatar(monster) > m_Range)
				continue;

			attackQueue.push(monster);
			attackSet.insert(monsterID);
		}

		if (attackQueue.empty())
			return;

		auto monster = attackQueue.front();
		attackQueue.pop();

		auto monsterID = monster->fields._runtimeID_k__BackingField;
		attackSet.erase(monsterID);

		auto combat = app::BaseEntity_GetBaseCombat(monster, *app::BaseEntity_GetBaseCombat__MethodInfo);
		auto maxHP = app::SafeFloat_GetValue(nullptr, combat->fields._combatProperty_k__BackingField->fields.maxHP, nullptr);

		auto crashEvt = app::CreateCrashEvent(nullptr, *app::CreateCrashEvent__MethodInfo);
		app::EvtCrash_Init(crashEvt, monsterID, nullptr);
		crashEvt->fields.maxHp = maxHP;
		crashEvt->fields.velChange = 1000;
		crashEvt->fields.hitPos = app::BaseEntity_GetAbsolutePosition(monster, nullptr);

		app::EventManager_FireEvent(eventManager, reinterpret_cast<app::BaseEvent*>(crashEvt), false, nullptr);

		monsterRepeatTimeMap[monsterID] = currentTime + (int)m_RepeatDelay + distribution(generator);

		nextAttackTime = currentTime + (int)m_AttackDelay + distribution(generator);
	}
}

