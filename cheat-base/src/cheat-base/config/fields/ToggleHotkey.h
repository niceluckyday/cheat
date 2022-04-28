#pragma once

#include <cheat-base/Hotkey.h>
#include <cheat-base/config/internal/FieldBase.h>

namespace config
{
	class ToggleHotkey
	{
	public:
		bool enabled;
		Hotkey hotkey;

		operator bool&()
		{
			return enabled;
		}

		bool operator==(const ToggleHotkey& rhs)
		{
			return rhs.enabled == enabled && rhs.hotkey == hotkey;
		}

		inline explicit ToggleHotkey(const Hotkey& hotkey) : enabled(false), hotkey(hotkey) { }

		ToggleHotkey(bool enabled) : enabled(enabled), hotkey()
		{

		}
	};

	namespace converters
	{
		// ToggleField
		template<>
		inline nlohmann::json ToJson(const ToggleHotkey& value)
		{
			if (value.hotkey.GetKeys().empty())
				return nlohmann::json(value.enabled);

			return {
				{"toggled", value.enabled},
				{"hotkey", ToJson(value.hotkey)}
			};
		}

		template<>
		inline void FromJson(ToggleHotkey& value, const nlohmann::json& jObject)
		{
			if (jObject.is_boolean())
			{
				value.enabled = jObject;
				value.hotkey = {};
				return;
			}

			value.enabled = jObject["toggled"];
			FromJson(value.hotkey, jObject["hotkey"]);
		}
	}

	// Okay, close your eyes and don't look at this mess. (Please)
	template<>
	class Field<ToggleHotkey> : public internal::FieldBase<ToggleHotkey>
	{
	public:
		using base = internal::FieldBase<ToggleHotkey>;
		using base::operator=;
		using base::base;

		operator bool() const
		{
			return value();
		}
	};
}