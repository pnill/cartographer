#pragma once
#include <codecvt>
#ifndef DATUM_INDEX_H
#define DATUM_INDEX_H
/*********************************************************************
* datum / DatumIndex
* 4 BYTE Tag Structure for any TagIndices of Halo 2 CacheFiles
**********************************************************************/
struct datum
{
	//Default Constructor
	datum()
	{
		this->Index = -1;
		this->Salt = -1;
	}
	//Copy Constructor
	datum(INT32 Identifier)
	{
		this->data = Identifier;
	}

	constexpr datum(short _index, short _salt) :
		Index(_index),
		Salt(_salt)
	{
	}

	static constexpr INT32 Null = 0xFFFFFFFF;

	//Returns true is datum is not Invalid
	inline bool IsNull() const
	{
		return this->Salt == -1 && this->Index == -1;
	}

	inline unsigned long ToInt() const
	{
		return data;
	}

	inline signed short ToAbsoluteIndex() const { return Index; };

	inline bool operator == (const datum &other) const
	{
		return this->data == other.data;
	}
	inline bool operator != (const datum &other) const
	{
		return this->data != other.data;
	}
	inline bool operator<(const short &Value) const
	{
		return this->Index < Value;
	}
	inline bool operator<(const datum &datum) const
	{
		return this->Index < datum.Index;
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
			signed short Index;
			signed short Salt;
		};
		unsigned long data;
	};
};
static_assert(sizeof(datum) == 4, "Invalid Size for struct (datum)");
#endif