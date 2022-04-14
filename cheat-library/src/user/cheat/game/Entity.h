#pragma once

namespace cheat::game
{
	class Entity
	{
	public:

		Entity(app::BaseEntity* rawEntity);

		std::string& name();

		app::BaseEntity* raw();
		uint32_t runtimeID();
		app::EntityType__Enum_1 type();

		app::Vector3 relativePosition();
		app::Vector3 absolutePosition();

		float distance(const app::Vector3& point);
		float distance(Entity* entity);
		float distance(app::BaseEntity* rawEntity);

		bool isGadget();
		bool isChest();
		bool isAvatar();

		bool isLoaded();

		void setRelativePosition(const app::Vector3& value);
		void setAbsolutePosition(const app::Vector3& value);
		
	private:

		app::BaseEntity* m_RawEntity;
		bool m_HasName;
		std::string m_Name;

		bool m_IsLoaded;
	};
}
