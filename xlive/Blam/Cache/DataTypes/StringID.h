#pragma once
#ifndef STRING_ID_H
#define STRING_ID_H
#include <codecvt>

/*********************************************************************
* string_id
* 4 BYTE Special Tag Structure for String Values
* Index 3 Bytes
* Length 1 Byte
**********************************************************************/
struct string_id
{
	string_id()
	{
		this->Handle = Empty;
	}
	string_id(UINT32 Value)
	{
		this->Handle = Empty;
		this->Handle = Value;
	}
	string_id(UINT Index, BYTE Length)
	{
		this->Handle = Empty;
		this->Handle = (UINT32)(Length << 24 | (Index & 0x00FFFFFF));
	}
	static const UINT32 Invalid = 0xFFFFFFFF;
	static const UINT32 Empty = 0;
	static const UINT MaxLength = 0xFF;
	static const UINT32 MaxIndex = 0xFFFFFF;
	//Returns Absolute String Index
	UINT ToIndex();
	//Returns String Length
	BYTE ToLength();
	//Check if String is InValid
	bool IsInvalid();
	//Check if String is Empty
	bool IsEmpty();
	//Return in StringFormat
	std::string ToString();
	//Get the String Linked with This SID
	std::string GetStringValue();

	void operator = (const UINT32 &Value);
	void operator = (const string_id &string_id);
	bool operator == (const UINT32 &Value);
	bool operator == (const string_id &string_id);
	bool operator != (const UINT32 &Value);
	bool operator != (const string_id &string_id);
private:
	UINT32 Handle;

};
static_assert(sizeof(string_id) == 4, "Invalid Size for struct (string_id)");


inline bool string_id::IsEmpty()
{
	return this->Handle == Empty;
}
inline bool string_id::IsInvalid()
{
	return this->Handle == Invalid;
}
inline UINT string_id::ToIndex()
{
	return this->Handle & 0x00FFFFFF;
}
inline BYTE string_id::ToLength()
{
	return (BYTE)(this->Handle & 0xFF000000);
}
inline void string_id::operator= (const UINT32 &Value)
{
	this->Handle = Value;
}
inline void string_id::operator=(const string_id &string_id)
{
	this->Handle = string_id.Handle;
}
inline bool string_id::operator== (const UINT32 &Value)
{
	return this->Handle == Value;
}
inline bool string_id::operator== (const string_id &string_id)
{
	return this->Handle = string_id.Handle;
}
inline bool string_id::operator!= (const UINT32 &Value)
{
	return this->Handle != Value;
}
inline bool string_id::operator!= (const string_id &string_id)
{
	return this->Handle != string_id.Handle;
}
inline std::string string_id::ToString()
{
	std::string val;
	val = this->Handle;
	return val;
}
#endif