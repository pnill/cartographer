#pragma once

#include "BlamPrimitiveType.h"

/*********************************************************************
* string_id
* 4 BYTE Special Tag Structure for String Values
* Index 3 Bytes
* Length 1 Byte
**********************************************************************/
struct string_id
{
	string_id() = default;

	constexpr string_id(uint32_t _value) :
		value(_value)
	{};

	constexpr string_id(uint32_t id, uint8_t length) :
		value(id | (length << 24))
	{
	};

	constexpr uint8_t get_length() const
	{
		return (value >> 24) & 0xFFu;
	}

	constexpr uint32_t get_id() const
	{
		return value & ~(0xffu << 24);
	}

	constexpr uint32_t get_packed() const
	{
		return value;
	}
	constexpr bool is_valid() const
	{
		return get_packed() != 0;
	}

	static const uint32_t Invalid = 0xFFFFFFFF;
	static const uint32_t Empty = 0;
	static const uint32_t MaxLength = 0xFF;
	static const uint32_t MaxIndex = 0xFFFFFF;
	

	void operator = (const uint32_t &Value);
	void operator = (const string_id &string_id);
	bool operator == (const uint32_t &Value);
	bool operator == (const string_id &string_id);
	bool operator != (const uint32_t &Value);
	bool operator != (const string_id &string_id);
private:
	uint32_t value;

};
CHECK_STRUCT_SIZE(string_id, 4);


inline void string_id::operator= (const uint32_t &Value)
{
	this->value = Value;
}
inline void string_id::operator=(const string_id &string_id)
{
	this->value = string_id.value;
}
inline bool string_id::operator== (const uint32_t &Value)
{
	return this->value == Value;
}
inline bool string_id::operator== (const string_id &string_id)
{
	return this->value = string_id.value;
}
inline bool string_id::operator!= (const uint32_t &Value)
{
	return this->value != Value;
}
inline bool string_id::operator!= (const string_id &string_id)
{
	return this->value != string_id.value;
}
