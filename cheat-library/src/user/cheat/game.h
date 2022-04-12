#pragma once

#include <Windows.h>
#include <string>
#include <vector>

#include <helpers.h>
#include <il2cpp-appdata.h>

namespace cheat::game
{
	template<class type>
	struct EntityFilterField
	{
		bool enabled;
		type value;
	};

	struct WaypointInfo
	{
		uint32_t sceneId = 0;
		uint32_t waypointId = 0;
		app::Vector3 position = {};
		app::MapModule_ScenePointData* data = nullptr;
	};

	struct SimpleEntityFilter
	{
		EntityFilterField<app::EntityType__Enum_1> typeFilter;
		EntityFilterField<std::vector<std::string>> nameFilter;
	};

	// Position utils
	app::Vector3 GetRelativePosition(app::BaseEntity* entity);
	void SetRelativePosition(app::BaseEntity* entity, app::Vector3 position);
	app::Vector3 GetAvatarRelativePosition();

	app::Vector3 GetAbsolutePosition(app::BaseEntity* entity);
	void SetAbsolutePosition(app::BaseEntity* entity, app::Vector3 position);
	app::Vector3 GetAvatarAbsolutePosition();

	std::string GetEntityName(app::BaseEntity* entity);

	app::BaseEntity* GetAvatarEntity();
	uint32_t GetAvatarRuntimeId();
	bool IsAvatarEntity(app::BaseEntity* entity);
	void SetAvatarRelativePosition(app::Vector3 position);
	float GetDistToAvatar(app::Vector3 relPos);
	float GetDistToAvatar(app::BaseEntity* entity);
	app::CameraEntity* GetMainCameraEntity();

	std::vector<app::BaseEntity*> GetEntities();
	app::BaseEntity* GetEntityByRuntimeId(uint32_t runtimeId);

	// Entity filters
	typedef bool (*FilterFunc)(app::BaseEntity* entity);

	std::vector<app::BaseEntity*> FindEntities(FilterFunc func);
	std::vector<app::BaseEntity*> FindEntities(const SimpleEntityFilter& filter);

	app::BaseEntity* FindNearestEntity(FilterFunc func);
	app::BaseEntity* FindNearestEntity(const SimpleEntityFilter& filter);
	app::BaseEntity* FindNearestEntity(std::vector<app::BaseEntity*>& entities);

	bool IsEntityFilterValid(app::BaseEntity* entity, const SimpleEntityFilter& filter);

	const SimpleEntityFilter& GetFilterCrystalShell();
	const SimpleEntityFilter& GetFilterChest();
	const SimpleEntityFilter& GetMonsterFilter();

	bool IsEntityCrystalShell(app::BaseEntity* entity);

	WaypointInfo FindNearestWaypoint(app::Vector3& position, uint32_t targetId = 0);
	static std::vector<WaypointInfo> GetUnlockedWaypoints(uint32_t targetId = 0);
	uint32_t GetCurrentMapSceneID();
	uint32_t GetCurrentPlayerSceneID();

	app::GadgetEntity* GetGadget(uint32_t runtimeID);
	app::GadgetEntity* GetGadget(app::BaseEntity* entity);
	bool IsEntityGadget(app::BaseEntity* entity);

	namespace chest
	{
		enum class ItemType
		{
			None,
			Chest,
			Investigate,
			BookPage,
			BGM
		};

		enum class ChestState
		{
			Invalid,
			None,
			Locked,
			InRock,
			Frozen,
			Bramble,
			Trap
		};

		enum class ChestRarity
		{
			Unknown,
			Common,
			Exquisite,
			Precious,
			Luxurious,
			Remarkable
		};

		ItemType GetItemType(const std::string& entityName);
		ChestRarity GetChestRarity(const std::string& entityName);
		ChestState GetChestState(app::BaseEntity* entity);
		ImColor GetChestColor(ItemType itemType, ChestRarity rarity = ChestRarity::Unknown);
		std::string GetChestMinName(ItemType itemType, ChestRarity rarity = ChestRarity::Unknown);
	}
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
