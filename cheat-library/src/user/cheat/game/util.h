#pragma once

#include <il2cpp-appdata.h>
#include <vector>

#include <helpers.h>

#include "Entity.h"
#include "SimpleFilter.h"

namespace cheat::game
{
	struct WaypointInfo
	{
		uint32_t sceneId = 0;
		uint32_t waypointId = 0;
		app::Vector3 position = {};
		app::MapModule_ScenePointData* data = nullptr;
	};

	WaypointInfo FindNearestWaypoint(app::Vector3& position, uint32_t targetId = 0);
	static std::vector<WaypointInfo> GetUnlockedWaypoints(uint32_t targetId = 0);
	uint32_t GetCurrentMapSceneID();
	uint32_t GetCurrentPlayerSceneID();

	app::GadgetEntity* GetGadget(uint32_t runtimeID);
	app::GadgetEntity* GetGadget(Entity* entity);
	app::GadgetEntity* GetGadget(app::BaseEntity* entity);

	const SimpleFilter& GetFilterCrystalShell();
	const SimpleFilter& GetFilterChest();
	const SimpleFilter& GetMonsterFilter();

	game::Entity* FindNearestEntity(const IEntityFilter& filter);
	game::Entity* FindNearestEntity(std::vector<game::Entity*>& entities);

	template<class T>
	T* CastTo(void* pObject, void* pClass)
	{
		auto object = reinterpret_cast<app::Object*>(pObject);
		if (object == nullptr || object->klass == nullptr || object->klass != pClass)
			return nullptr;

		return reinterpret_cast<T*>(object);
	}

	template<class T>
	T* GetLCPlugin(app::BaseComponentPlugin* plugin, void* pClass)
	{
		return CastTo<T>(plugin, pClass);
	}

	template<class T>
	T* GetLCPlugin(app::BaseComponent* component, void* pClass)
	{
		if (component == nullptr || component->fields._pluginList == nullptr)
			return nullptr;

		auto componentList = ToUniList(component->fields._pluginList, app::BaseComponentPlugin*);
		for (auto& plugin : *componentList)
		{
			T* value = GetLCPlugin<T>(plugin, pClass);
			if (value != nullptr)
				return value;
		}
		return nullptr;
	}

	template<class T>
	T* GetLCPlugin(app::BaseEntity* entity, void* pClass)
	{
		if (entity == nullptr)
			return nullptr;

		auto logicComponentsRaw = app::BaseEntity_GetAllLogicComponents(entity, nullptr);
		auto logicComponents = ToUniList(logicComponentsRaw, app::BaseComponent*);
		if (logicComponents == nullptr)
			return nullptr;

		for (auto& component : *logicComponents)
		{
			T* value = GetLCPlugin<T>(component, pClass);
			if (value != nullptr)
				return value;
		}
		return nullptr;
	}

}
