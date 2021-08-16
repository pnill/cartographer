#pragma once

#include "Blam\Common\Common.h"

#define NONE -1

// floating point definitions
typedef float real;

struct datum
{
	//Default constructors
	datum()
	{
		operator=(datum(NONE));
	}

	datum(int index)
	{
		this->data = index;
	}

	constexpr datum(short _index, short _salt) :
		Index(_index),
		Salt(_salt)
	{
	}

	//Returns true is datum is not Invalid
	inline bool IsNull() const
	{
		return this->Salt == NONE && this->Index == NONE;
	}

	inline unsigned long ToInt() const
	{
		return data;
	}

	inline signed short ToAbsoluteIndex() const { return Index; };

	inline void operator=(const datum &other)
	{
		this->data = other.data;
	}
	inline bool operator == (const datum &other) const
	{
		return this->data == other.data;
	}
	inline bool operator != (const datum &other) const
	{
		return this->data != other.data;
	}
	inline bool operator<(const short &other) const
	{
		return this->Index < other;
	}
	inline bool operator<(const datum &other) const
	{
		return this->Index < other.Index;
	}

	inline std::string ToString() const
	{
		std::stringstream ss;
		ss << "Index: " << std::hex << Index <<
			" Salt: " << std::hex << Salt <<
			" Datum: " << std::hex << data;
		return ss.str();
	}

	union {
		struct {
			short Index;
			short Salt;
		};
		int data;
	};
};
CHECK_STRUCT_SIZE(datum, 4);

#define DATUM_NONE datum(NONE)
