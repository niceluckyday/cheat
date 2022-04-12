#pragma once
#include "ConfigEntry.h"

namespace config::field
{
	template<class T>
	class FieldBase : public ConfigEntry
	{
	public:
		// Quite interesting shit with pointer value, seems like memory leak (and it is)
		//   but implied that it will use rarely and have one instance
		FieldBase(const std::string friendlyName, const std::string name, const std::string section, T defaultValue)
			: ConfigEntry(friendlyName, name, section),
			fieldPtr(new T(defaultValue))
		{
		}

		T value() const
		{
			return *fieldPtr;
		}

		T* valuePtr() const
		{
			return fieldPtr;
		}

		operator T() const {
			return value();
		}

		operator T* () const {
			return valuePtr();
		}

		void operator=(const T& other)
		{
			*fieldPtr = other;
			ChangedEvent(this);
		}

		virtual bool Check()
		{
			ChangedEvent(this);
			return true;
		}

	protected:
		T* fieldPtr;
	};
}