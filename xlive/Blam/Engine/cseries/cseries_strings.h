#pragma once

#include "Blam/Engine/text/unicode.h"

#define MAXIMUM_STRING_SIZE 262144

size_t csstrnlen(char* string, size_t size);
int csstricmp(const char* s1, const char* s2);

/*********************************************************************
* static_string
* Storing and Manipulating Character Sequences(Basic String)
**********************************************************************/

template<int length>
struct static_string
{
	char text[length];
	char* get_string();
	size_t length();
	void operator = (const char* Value);
	void operator = (const static_string Value);
};

/*********************************************************************
* static_string32
* Storing and Manipulating Character Sequences (static_string<32>)
**********************************************************************/
typedef static_string<32>  static_string32;

/*********************************************************************
* static_string64
* Storing and Manipulating Character Sequences(static_string<64>)
**********************************************************************/
typedef static_string<64>  static_string64;

/*********************************************************************
* static_string128
* Storing and Manipulating Character Sequences(static_string<128>)
**********************************************************************/
typedef static_string<128> static_string128;

/*********************************************************************
* static_string256
* Storing and Manipulating Character Sequences(static_string<256>)
**********************************************************************/
typedef static_string<256> static_string256;

/*********************************************************************
* static_string512
* Storing and Manipulating Character Sequences(static_string<512>)
**********************************************************************/
typedef static_string<512> static_string512;

/*********************************************************************
* static_wchar_string
* Storing and Manipulating Unicode Character Sequences(Wide static_string)
**********************************************************************/
template<int length>
struct static_wchar_string
{
	wchar_t text[length];
	wchar_t* get_string();
	size_t length();
	void operator = (const wchar_t* Value);
	void operator = (const static_wchar_string Value);
};

/*********************************************************************
* static_wchar_string32
* Storing and Manipulating Unicode Character Sequences(static_wchar_string<32>)
**********************************************************************/
typedef static_wchar_string<32>  static_wchar_string32;

/*********************************************************************
* static_wchar_string64
* Storing and Manipulating Unicode Character Sequences(static_wchar_string<64>)
**********************************************************************/
typedef static_wchar_string<64>  static_wchar_string64;

/*********************************************************************
* static_wchar_string128
* Storing and Manipulating Unicode Character Sequences(static_wchar_string<128>)
**********************************************************************/
typedef static_wchar_string<128> static_wchar_string128;

/*********************************************************************
* static_wchar_string256
* Storing and Manipulating Unicode Character Sequences(static_wchar_string<256>)
**********************************************************************/
typedef static_wchar_string<256> static_wchar_string256;

template<int T>
inline char* static_string<T>::get_string()
{
	return this->text;
}

template<int T>
inline size_t static_string<T>::length()
{
	return csstrnlen(this->text, T);
}

template<int T>
inline void static_string<T>::operator= (const char* Value)
{
	strcpy(this->text, Value);
}
template<int T>
inline void static_string<T>::operator= (const static_string Value)
{
	strcpy(this->text, Value.text);
}

template<int T>
inline wchar_t* static_wchar_string<T>::get_string()
{
	return this->text;
}

template<int T>
inline size_t static_wchar_string<T>::length()
{
	return ustrnlen(this->text, T);
}

template<int T>
inline void static_wchar_string<T>::operator= (const wchar_t* Value)
{
	wcscpy(this->text, Value);
}
template<int T>
inline void static_wchar_string<T>::operator= (const static_wchar_string Value)
{
	wcscpy(this->text, Value.text);
}
