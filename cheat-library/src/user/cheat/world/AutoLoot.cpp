#include "pch-il2cpp.h"
#include "AutoLoot.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>

namespace cheat::feature 
{
	static void LCSelectPickup_AddInteeBtnByID_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method);
	static bool LCSelectPickup_IsInPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method);
	static bool LCSelectPickup_IsOutPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method);

    AutoLoot::AutoLoot() : Feature(),
        NF(f_Enabled,        "Auto loot",          "AutoLoot", false),
        NF(f_DelayTime,      "Delay time (in ms)", "AutoLoot", 150),
        NF(f_UseCustomRange, "Use custom pickup range",   "AutoLoot", false),
        NF(f_CustomRange,    "Pickup Range",       "AutoLoot", 5.0f),
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
        static const FeatureGUIInfo info{ "Auto Loot", "World", true };
        return info;
    }

    void AutoLoot::DrawMain()
    {
		ConfigWidget("Enabled", f_Enabled, "Loots dropped items.\n" \
            "Note: Custom range and low delay times are high-risk features.\n" \
			"Abuse will definitely merit a ban.");
		ConfigWidget("Delay Time (ms)", f_DelayTime, 1, 0, 1000, "Delay (in ms) beetwen looting items.\n" \
            "Values under 200ms are unsafe.");
		ConfigWidget("Use Custom Pickup Range", f_UseCustomRange, "Enable custom pickup range.\n" \
            "Using this feature is not recommended, as it is easily detected by the server.");
		ConfigWidget("Range (m)", f_CustomRange, 0.1f, 0.5f, 60.0f, "Modifies pickup range to this value (in meters).");
    }

    bool AutoLoot::NeedStatusDraw() const
{
        return f_Enabled;
    }

    void AutoLoot::DrawStatus() 
    {
		ImGui::Text("Auto Loot [%dms%s]",
			f_DelayTime.value(),
			f_UseCustomRange ? fmt::format("|{:.1f}m ", f_CustomRange.value()).c_str() : "");
    }

    AutoLoot& AutoLoot::GetInstance()
    {
        static AutoLoot instance;
        return instance;
    }

	bool AutoLoot::OnCreateButton(app::BaseEntity* entity)
	{
		if (!f_Enabled)
			return false;

		auto itemModule = GET_SINGLETON(ItemModule);
		if (itemModule == nullptr)
			return false;

		auto entityId = entity->fields._runtimeID_k__BackingField;
		if (f_DelayTime == 0)
		{
			app::ItemModule_PickItem(itemModule, entityId, nullptr);
			return true;
		}

		toBeLootedItems.push(entityId);
		return false;
	}

	void AutoLoot::OnGameUpdate()
	{
		auto currentTime = util::GetCurrentTimeMillisec();
		if (currentTime < nextLootTime)
			return;

		auto entityManager = GET_SINGLETON(EntityManager);
		if (entityManager == nullptr)
			return;

		auto entityId = toBeLootedItems.pop();
		if (!entityId)
			return;

		auto itemModule = GET_SINGLETON(ItemModule);
		if (itemModule == nullptr)
			return;

		auto entity = app::EntityManager_GetValidEntity(entityManager, *entityId, nullptr);
		if (entity == nullptr)
			return;

		app::ItemModule_PickItem(itemModule, *entityId, nullptr);
		nextLootTime = currentTime + (int)f_DelayTime;
	}

	void AutoLoot::OnCheckIsInPosition(bool& result, app::BaseEntity* entity)
	{
		if (f_Enabled && f_UseCustomRange)
		{
			auto& manager = game::EntityManager::instance();
			result = manager.avatar()->distance(entity) < f_CustomRange;
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

