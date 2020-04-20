#pragma once
#include <codecvt>
#ifndef TAG_STRINGS_H
#define TAG_STRINGS_H
/*********************************************************************
* tag_string
* Storing and Manipulating Character Sequences(Basic String)
**********************************************************************/
template<int length>
struct tag_string
{
	char Text[length];
	std::string get_tag_string();
	void operator = (const char* Value);
	void operator = (const tag_string Value);
};
/*********************************************************************
* tag_string32
* Storing and Manipulating Character Sequences (tag_string<32>)
**********************************************************************/
typedef tag_string<32>  tag_string32;
/*********************************************************************
* tag_string64
* Storing and Manipulating Character Sequences(tag_string<64>)
**********************************************************************/
typedef tag_string<64>  tag_string64;
/*********************************************************************
* tag_string128
* Storing and Manipulating Character Sequences(tag_string<128>)
**********************************************************************/
typedef tag_string<128> tag_string128;
/*********************************************************************
* tag_string256
* Storing and Manipulating Character Sequences(tag_string<256>)
**********************************************************************/
typedef tag_string<256> tag_string256;
/*********************************************************************
* tag_string512
* Storing and Manipulating Character Sequences(tag_string<512>)
**********************************************************************/
typedef tag_string<512> tag_string512;


/*********************************************************************
* tag_unicode
* Storing and Manipulating Unicode Character Sequences(Wide tag_string)
**********************************************************************/
template<int length>
struct tag_unicode
{
	wchar_t Text[length];
	std::wstring get_tag_string();
	void operator = (const wchar_t* Value);
	void operator = (const tag_unicode Value);
};
/*********************************************************************
* tag_unicode32
* Storing and Manipulating Unicode Character Sequences(tag_unicode<32>)
**********************************************************************/
typedef tag_unicode<32>  tag_unicode32;
/*********************************************************************
* tag_unicode64
* Storing and Manipulating Unicode Character Sequences(tag_unicode<64>)
**********************************************************************/
typedef tag_unicode<64>  tag_unicode64;
/*********************************************************************
* tag_unicode128
* Storing and Manipulating Unicode Character Sequences(tag_unicode<128>)
**********************************************************************/
typedef tag_unicode<128> tag_unicode128;
/*********************************************************************
* tag_unicode256
* Storing and Manipulating Unicode Character Sequences(tag_unicode<256>)
**********************************************************************/
typedef tag_unicode<256> tag_unicode256;


template<int T>
inline std::string  tag_string<T>::get_tag_string()
{
	std::string k = this->Text;
	return k;
}
template<int T>
inline void tag_string<T>::operator= (const char* Value)
{
	this->Text = Value;
}
template<int T>
inline void tag_string<T>::operator= (const tag_string Value)
{
	this->Text = Value.Text;
}

template<int T>
inline std::wstring  tag_unicode<T>::get_tag_string()
{
	std::wstring k = this->Text;
	return k;
}
template<int T>
inline void tag_unicode<T>::operator= (const wchar_t* Value)
{
	this->Text = Value;
}
template<int T>
inline void tag_unicode<T>::operator= (const tag_unicode Value)
{
	this->Text = Value.Text;
}
#endif
