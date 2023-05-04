#pragma once
#include <string>

/*********************************************************************
* static_string
* Storing and Manipulating Character Sequences(Basic String)
**********************************************************************/

template<int length>
struct static_string
{
	char text[length];
	std::string get_static_string();
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
	std::wstring get_static_string();
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
inline std::string static_string<T>::get_static_string()
{
	std::string k = this->text;
	return k;
}
template<int T>
inline void static_string<T>::operator= (const char* Value)
{
	strcpy(this->text, Value);
	//this->text = Value;
}
template<int T>
inline void static_string<T>::operator= (const static_string Value)
{
	strcpy(this->text, Value.text);
	//this->text = Value.text;
}

template<int T>
inline std::wstring static_wchar_string<T>::get_static_string()
{
	std::wstring k = this->text;
	return k;
}
template<int T>
inline void static_wchar_string<T>::operator= (const wchar_t* Value)
{
	wcscpy(this->text, Value);
	//this->text = Value;
}
template<int T>
inline void static_wchar_string<T>::operator= (const static_wchar_string Value)
{
	wcscpy(this->text, Value.text);
	//this->text = Value.text;
}
