#include "pch-il2cpp.h"
#include "Entity.h"

#include <helpers.h>
#include "EntityManager.h"

namespace cheat::game
{

	Entity::Entity(app::BaseEntity* rawEntity) : m_RawEntity(rawEntity), m_Name({}), m_HasName(false)
	{
	}

	std::string& Entity::name()
	{
		if (m_HasName || m_RawEntity == nullptr || !isLoaded())
			return m_Name;

		auto name = il2cppi_to_string(app::BaseEntity_ToStringRelease(m_RawEntity, nullptr));
		m_Name = name;
		m_HasName = true;
		return m_Name;
	}

	app::BaseEntity* Entity::raw()
	{
		return m_RawEntity;
	}

	uint32_t Entity::runtimeID()
	{
		if (m_RawEntity == nullptr)
			return 0;

		return m_RawEntity->fields._runtimeID_k__BackingField;
	}

	app::EntityType__Enum_1 Entity::type()
	{
		if (m_RawEntity == nullptr)
			return app::EntityType__Enum_1::None;

		return m_RawEntity->fields.entityType;
	}

	app::Vector3 Entity::relativePosition()
	{
		if (m_RawEntity == nullptr)
			return {};

		return app::BaseEntity_GetRelativePosition(m_RawEntity, nullptr);
	}

	app::Vector3 Entity::absolutePosition()
	{
		if (m_RawEntity == nullptr)
			return {};

		return app::BaseEntity_GetAbsolutePosition(m_RawEntity, nullptr);
	}

	float Entity::distance(Entity* entity)
	{
		if (entity == nullptr)
			return 1000;

		return distance(entity->relativePosition());
	}

	float Entity::distance(app::BaseEntity* rawEntity)
	{
		if (rawEntity == nullptr)
			return 0;

		auto point = app::BaseEntity_GetRelativePosition(rawEntity, nullptr);
		return distance(point);
	}

	float Entity::distance(const app::Vector3& point)
	{
		if (m_RawEntity == nullptr)
			return 0;

		auto dist = app::Vector3_Distance(nullptr, relativePosition(), point, nullptr);
		return dist;
	}

	bool Entity::isGadget()
	{
		if (m_RawEntity == nullptr)
			return false;

		return m_RawEntity->fields.entityType == app::EntityType__Enum_1::Gadget ||
			m_RawEntity->fields.entityType == app::EntityType__Enum_1::Bullet ||
			m_RawEntity->fields.entityType == app::EntityType__Enum_1::Field;
	}

	bool Entity::isChest()
	{
		if (m_RawEntity == nullptr)
			return false;

		return m_RawEntity->fields.entityType == app::EntityType__Enum_1::Chest;
	}

	bool Entity::isAvatar()
	{
		if (m_RawEntity == nullptr)
			return false;

		auto avatar = EntityManager::instance().avatar();
		if (avatar->raw() == nullptr)
			return false;

		return avatar->raw() == m_RawEntity;
	}

	void Entity::setRelativePosition(const app::Vector3& value)
	{
		if (m_RawEntity == nullptr)
			return;

		app::BaseEntity_SetRelativePosition(m_RawEntity, value, true, nullptr);
	}

	void Entity::setAbsolutePosition(const app::Vector3& value)
	{
		if (m_RawEntity == nullptr)
			return;

		app::BaseEntity_SetAbsolutePosition(m_RawEntity, value, true, nullptr);
	}

	bool Entity::isLoaded()
	{
		if (m_RawEntity == nullptr || !app::BaseEntity_IsActive(m_RawEntity, nullptr))
			return false;

		m_IsLoaded = m_IsLoaded || app::BaseEntity_get_rootGameObject(m_RawEntity, nullptr);
		return m_IsLoaded;
	}

}
