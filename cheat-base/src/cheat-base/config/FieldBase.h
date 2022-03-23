#pragma once
#include "ConfigEntry.h"

namespace config::field
{
	template<class T>
	class FieldBase : public ConfigEntry
	{
	public:
		FieldBase(const std::string friendlyName, const std::string name, const std::string section, T defaultValue)
			: ConfigEntry(friendlyName, name, section),
			fieldValue(new T(defaultValue)), prevValue(new T(defaultValue))
		{
		}

		~FieldBase()
		{
			delete fieldValue;
			delete prevValue;
		}

		T value() const
		{
			return *fieldValue;
		}

		T* valuePtr() const
		{
			return fieldValue;
		}

		operator T() const {
			return value();
		}

		operator T* () const {
			return valuePtr();
		}

		void operator=(const T& other)
		{
			*fieldValue = other;
			*prevValue = other;
			ChangedEvent(this);
		}

		virtual bool Check()
		{
			if (*prevValue == *fieldValue)
				return false;

			*prevValue = *fieldValue;
			ChangedEvent(this);
			return true;
		}

	private:
		T* fieldValue;
		T* prevValue;
	};
}