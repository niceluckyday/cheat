#pragma once
#include <string>
#include <cheat-base/Event.h>
#include <nlohmann/json.hpp>

namespace config::internal
{
	class FieldEntry
	{
	public:
		FieldEntry(const std::string& friendlyName, const std::string& name, const std::string& sectionName, bool multiProfile = false)
			: m_FriendName(friendlyName), m_Name(name), m_Section(sectionName), m_MultiProfile(multiProfile), m_Container(nullptr) {}

		TEvent<FieldEntry*> ChangedEvent;
		virtual void FireChanged()
		{
			ChangedEvent(this);
		}

		virtual nlohmann::json ToJson() = 0;
		virtual void FromJson(const nlohmann::json& value) = 0;
		virtual void Reset() = 0;

		bool IsShared() const
		{
			return m_MultiProfile;
		}

		std::string GetName() const
		{
			return m_Name;
		}

		std::string GetFriendName() const
		{
			return m_FriendName;
		}

		std::string GetSection() const
		{
			return m_Section;
		}

		nlohmann::json* GetContainer() const
		{
			return m_Container;
		}

		void SetContainer(nlohmann::json* newContainer)
		{
			m_Container = nullptr;
		}

	protected:
		std::string m_Name;
		std::string m_FriendName;
		std::string m_Section;
		bool m_MultiProfile;

		nlohmann::json* m_Container;
	};
}