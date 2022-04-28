#pragma once

#include <magic_enum.hpp>

namespace config
{

	template<typename T>
	class Enum
	{
	public:
		Enum(T enumValue)
		{
			static_assert(std::is_enum<T>::value, "Must be an enum type");
			m_Value = enumValue;
		}

		T value() const
		{
			return m_Value;
		}

		T* pointer() const
		{
			return const_cast<T*>(&m_Value);
		}

		operator T()
		{
			return value();
		}

		T* operator&()
		{
			return pointer();
		}

		uint32_t raw() const
		{
			return static_cast<uint32_t>(m_Value);
		}

		Enum& operator=(const T& other)
		{
			static_assert(std::is_enum<T>::value, "Must be an enum type");
			m_Value = other;
			return *this;
		}

		Enum& operator=(const uint32_t& other)
		{
			m_Value = static_cast<T>(other);
			return *this;
		}

	private:
		T m_Value;
	};

	namespace converters
	{

	}

	//// Okay, close your eyes and don't look at this mess. (Please)
	//template <typename K>
	//class Field<Enum<K>> : public internal::FieldBase<Enum<K>>
	//{
	//public:
	//	using base = internal::FieldBase<Enum<K>>;
	//	using base::operator=;
	//	using base::base;

	//	operator T() const
	//	{
	//		return base::value();
	//	}
	//};
}