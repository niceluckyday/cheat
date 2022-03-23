#include "pch-il2cpp.h"
#include "AutoLoot.h"

#include <imgui.h>
#include <common/util.h>
#include <helpers.h>
#include <gui/gui-util.h>
#include <common/HookManager.h>
#include <cheat/events.h>

namespace cheat::feature 
{
	static void LCSelectPickup_AddInteeBtnByID_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method);
	static bool LCSelectPickup_IsInPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method);
	static bool LCSelectPickup_IsOutPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method);

    AutoLoot::AutoLoot() : Feature(),
        NF(m_Enabled,        "Auto loot",          "AutoLoot", false),
        NF(m_DelayTime,      "Delay time (in ms)", "AutoLoot", 150),
        NF(m_UseCustomRange, "Use custom range",   "AutoLoot", false),
        NF(m_CustomRange,    "Custom range",       "AutoLoot", 5.0f),
		toBeLootedItems(), nextLootTime(0)
    {
		// Auto loot
		HookManager::install(app::LCSelectPickup_AddInteeBtnByID, LCSelectPickup_AddInteeBtnByID_Hook);
		HookManager::install(app::LCSelectPickup_IsInPosition, LCSelectPickup_IsInPosition_Hook);
		HookManager::install(app::LCSelectPickup_IsOutPosition, LCSelectPickup_IsOutPosition_Hook);

		events::GameUpdateEvent += MY_METHOD_HANDLER(AutoLoot::OnGameUpdate);
	}

    const FeatureGUIInfo& AutoLoot::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "Auto loot", "World", true };
        return info;
    }

    void AutoLoot::DrawMain()
    {
		ConfigWidget("Enabled", m_Enabled, "Automatic loot dropped items.\n\
            Note: Use custom range or decreasing delay time can be unsafe.");
		ConfigWidget(m_DelayTime, 1, 0, 1000, "Delay (in milliseconds) beetwen looting items.\n\
            Value under 200ms can be unsafe.");
		ConfigWidget(m_UseCustomRange, "Enabling custom looting range.\n\
            Use this feature not recommended, it can be easily detected by server.");
		ConfigWidget(m_CustomRange, 0.1f, 0.5f, 60.0f, "Custom looting range. It's applying only if use custom range enabled.");
    }

    bool AutoLoot::NeedStatusDraw() const
{
        return m_Enabled;
    }

    void AutoLoot::DrawStatus() 
    {
        ImGui::Text("Auto loot [%d ms]", m_DelayTime.value());
    }

    AutoLoot& AutoLoot::GetInstance()
    {
        static AutoLoot instance;
        return instance;
    }

	bool AutoLoot::OnCreateButton(app::BaseEntity* entity)
	{
		if (!m_Enabled)
			return false;

		auto itemModule = GetSingleton(ItemModule);
		if (itemModule == nullptr)
			return false;

		auto entityId = entity->fields._runtimeID_k__BackingField;
		if (m_DelayTime == 0)
		{
			app::ItemModule_PickItem(itemModule, entityId, nullptr);
			return true;
		}

		toBeLootedItems.push(entityId);
		return false;
	}

	void AutoLoot::OnGameUpdate()
	{
		auto currentTime = GetCurrentTimeMillisec();
		if (currentTime < nextLootTime)
			return;

		auto entityManager = GetSingleton(EntityManager);
		if (entityManager == nullptr)
			return;

		auto entityId = toBeLootedItems.pop();
		if (!entityId)
			return;

		auto itemModule = GetSingleton(ItemModule);
		if (itemModule == nullptr)
			return;

		auto entity = app::EntityManager_GetValidEntity(entityManager, *entityId, nullptr);
		if (entity == nullptr)
			return;

		app::ItemModule_PickItem(itemModule, *entityId, nullptr);
		nextLootTime = currentTime + (int)m_DelayTime;
	}

	void AutoLoot::OnCheckIsInPosition(bool& result, app::BaseEntity* entity)
	{
		if (m_Enabled && m_UseCustomRange)
		{
			auto avatarPos = GetAvatarRelativePosition();
			auto entityPos = GetRelativePosition(entity);
			result = app::Vector3_Distance(nullptr, avatarPos, entityPos, nullptr) < m_CustomRange;
		}
	}

	static void LCSelectPickup_AddInteeBtnByID_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method)
	{
		AutoLoot& autoLoot = AutoLoot::GetInstance();
		bool canceled = autoLoot.OnCreateButton(entity);
		if (!canceled)
			callOrigin(LCSelectPickup_AddInteeBtnByID_Hook, __this, entity, method);
	}

	static bool LCSelectPickup_IsInPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method)
	{
		bool result = callOrigin(LCSelectPickup_IsInPosition_Hook, __this, entity, method);

		AutoLoot& autoLoot = AutoLoot::GetInstance();
		autoLoot.OnCheckIsInPosition(result, entity);

		return result;
	}

	static bool LCSelectPickup_IsOutPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method)
	{
		bool result = callOrigin(LCSelectPickup_IsOutPosition_Hook, __this, entity, method);

		AutoLoot& autoLoot = AutoLoot::GetInstance();
		autoLoot.OnCheckIsInPosition(result, entity);

		return result;
	}
}

