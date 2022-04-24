#include "pch-il2cpp.h"
#include "KillAura.h"

#include <helpers.h>
#include <algorithm>

#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>
#include <cheat/game/filters.h>

namespace cheat::feature 
{
	static void BaseMoveSyncPlugin_ConvertSyncTaskToMotionInfo_Hook(app::BaseMoveSyncPlugin* __this, MethodInfo* method);

    KillAura::KillAura() : Feature(),
        NF(m_Enabled,      "Kill aura",                 "KillAura", false),
		NF(m_DamageMode,   "Damage mode",               "Damage mode", false),
		NF(m_InstantDeathMode,   "Instant death",       "Instant death", false),
        NF(m_OnlyTargeted, "Only targeted",             "KillAura", true),
        NF(m_Range,        "Range",                     "KillAura", 15.0f),
        NF(m_AttackDelay,  "Attack delay time (in ms)", "KillAura", 100),
        NF(m_RepeatDelay,  "Repeat delay time (in ms)", "KillAura", 1000)
    { 
		events::GameUpdateEvent += MY_METHOD_HANDLER(KillAura::OnGameUpdate);
		HookManager::install(app::BaseMoveSyncPlugin_ConvertSyncTaskToMotionInfo, BaseMoveSyncPlugin_ConvertSyncTaskToMotionInfo_Hook);
	}

    const FeatureGUIInfo& KillAura::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "Kill Aura", "World", true };
        return info;
    }

    void KillAura::DrawMain()
    {
		ConfigWidget("Enable Kill Aura", m_Enabled, "Enables kill aura. Need to choose a mode to work.");
		ImGui::SameLine();
		ImGui::TextColored(ImColor(255, 165, 0, 255), "Choose any or both modes below.");
		if (m_Enabled) {
			ConfigWidget("Crash Damage Mode", m_DamageMode, "Kill aura causes crash damage for monster around you.");
			ConfigWidget("Instant Death Mode", m_InstantDeathMode, "Kill aura will attempt to instagib any valid target.");
			ImGui::SameLine();
			ImGui::TextColored(ImColor(255, 165, 0, 255), "Can get buggy with bosses like PMA and Hydro Hypo.");
			if (m_DamageMode || m_InstantDeathMode) {
				ConfigWidget(m_Range, 0.1f, 5.0f, 100.0f);
				ConfigWidget(m_OnlyTargeted, "If enabled, kill aura will only affect monsters targeting you.");
				ConfigWidget(m_AttackDelay, 1, 0, 1000, "Delay before next crash damage.");
				ConfigWidget(m_RepeatDelay, 1, 100, 2000, "Delay before crash damaging same monster.");
			}
		}
    }

    bool KillAura::NeedStatusDraw() const
	{
        return m_Enabled;
    }

    void KillAura::DrawStatus() 
    { 
        ImGui::Text("Kill Aura [%s]\n[%.01fm|%s|%dms|%dms]", 
			m_DamageMode && m_InstantDeathMode ? "Extreme" : m_DamageMode ? "Crash" : m_InstantDeathMode ? "Instant" : "None",
			m_Range.value(),
			m_OnlyTargeted ? "Aggro" : "All",
			m_AttackDelay.value(),
			m_RepeatDelay.value());
    }

    KillAura& KillAura::GetInstance()
    {
        static KillAura instance;
        return instance;
    }

	// Kill aura logic is just emulate monster fall crash, so simple but works.
	// Note. No work on mob with shield, maybe update like auto ore destroy.
	void KillAura::OnGameUpdate()
	{
		static std::default_random_engine generator;
		static std::uniform_int_distribution<int> distribution(-50, 50);

		static int64_t nextAttackTime = 0;
		static std::map<uint32_t, int64_t> monsterRepeatTimeMap;
		static std::queue<game::Entity*> attackQueue;
		static std::unordered_set<uint32_t> attackSet;

		if (!m_Enabled || !m_DamageMode)
			return;

		auto eventManager = GET_SINGLETON(EventManager);
		if (eventManager == nullptr || *app::CreateCrashEvent__MethodInfo == nullptr)
			return;

		auto currentTime = util::GetCurrentTimeMillisec();
		if (currentTime < nextAttackTime)
			return;

		auto& manager = game::EntityManager::instance();

		for (const auto& monster : manager.entities(game::filters::combined::Monsters))
		{
			auto monsterID = monster->runtimeID();

			if (attackSet.count(monsterID) > 0)
				continue;

			if (monsterRepeatTimeMap.count(monsterID) > 0 && monsterRepeatTimeMap[monsterID] > currentTime)
				continue;

			auto combat = monster->combat();
			if (combat == nullptr)
				continue;

			auto combatProp = combat->fields._combatProperty_k__BackingField;
			if (combatProp == nullptr)
				continue;

			auto maxHP = app::SafeFloat_GetValue(nullptr, combatProp->fields.maxHP, nullptr);
			auto isLockHp = combatProp->fields.islockHP == nullptr || app::FixedBoolStack_get_value(combatProp->fields.islockHP, nullptr);
			auto isInvincible = combatProp->fields.isInvincible == nullptr || app::FixedBoolStack_get_value(combatProp->fields.isInvincible, nullptr);
			auto HP = app::SafeFloat_GetValue(nullptr, combatProp->fields.HP, nullptr);
			if (maxHP < 10 || HP < 2 || isLockHp || isInvincible)
				continue;

			if (m_OnlyTargeted && combat->fields._attackTarget.runtimeID != manager.avatar()->runtimeID())
				continue;

			if (manager.avatar()->distance(monster) > m_Range)
				continue;

			attackQueue.push(monster);
			attackSet.insert(monsterID);
		}

		if (attackQueue.empty())
			return;

		auto monster = attackQueue.front();
		attackQueue.pop();

		if (!monster->isLoaded())
		{
			// If monster entity isn't active means that it was unloaded (it happen when player teleport or moving fast)
			// And we don't have way to get id
			// So better to clear all queue, to prevent memory leak
			// This happen rarely, so don't give any performance issues
			std::queue<game::Entity*> empty;
			std::swap(attackQueue, empty);

			attackSet.clear();
			return;
		}

		attackSet.erase(monster->runtimeID());

		auto combat = monster->combat();
		auto maxHP = app::SafeFloat_GetValue(nullptr, combat->fields._combatProperty_k__BackingField->fields.maxHP, nullptr);

		auto crashEvt = app::CreateCrashEvent(nullptr, *app::CreateCrashEvent__MethodInfo);
		app::EvtCrash_Init(crashEvt, monster->runtimeID(), nullptr);
		crashEvt->fields.maxHp = maxHP;
		crashEvt->fields.velChange = 1000;
		crashEvt->fields.hitPos = monster->absolutePosition();

		app::EventManager_FireEvent(eventManager, reinterpret_cast<app::BaseEvent*>(crashEvt), false, nullptr);

		monsterRepeatTimeMap[monster->runtimeID()] = currentTime + (int)m_RepeatDelay + distribution(generator);

		nextAttackTime = currentTime + (int)m_AttackDelay + distribution(generator);
	}

	static void OnSyncTask(app::BaseMoveSyncPlugin* moveSync)
	{
		KillAura& killAura = KillAura::GetInstance();
		if (!killAura.m_Enabled || !killAura.m_InstantDeathMode)
			return;

		auto& manager = game::EntityManager::instance();
		auto avatarID = manager.avatar()->runtimeID();
		auto entityID = moveSync->fields._.owner->fields.entityRuntimeID;

		if (entityID == avatarID)
			return;

		auto monster = manager.entity(entityID);
		auto combat = monster->combat();
		if (combat == nullptr)
			return;

		if (killAura.m_OnlyTargeted && combat->fields._attackTarget.runtimeID != avatarID)
			return;

		if (manager.avatar()->distance(monster) > killAura.m_Range)
			return;

		moveSync->fields.moveSyncTask.position.x = 1000000.0f;
	}

	static void BaseMoveSyncPlugin_ConvertSyncTaskToMotionInfo_Hook(app::BaseMoveSyncPlugin* __this, MethodInfo* method)
	{
		OnSyncTask(__this);
		callOrigin(BaseMoveSyncPlugin_ConvertSyncTaskToMotionInfo_Hook, __this, method);
	}
}

