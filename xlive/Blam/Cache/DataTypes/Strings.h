#pragma once
#include <codecvt>
namespace Blam
{
	namespace Cache
	{
		namespace DataTypes
		{

			/*********************************************************************
			* Blam::Cache::DataTypes::String
			* Storing and Manipulating Character Sequences(Basic String)
			**********************************************************************/
			template<int length>
			struct String
			{
				char Text[length];
				std::string GetString();
				void operator = (const char* Value);
				void operator = (const String Value);
			};
			/*********************************************************************
			* Blam::Cache::DataTypes::String32
			* Storing and Manipulating Character Sequences (String<32>)
			**********************************************************************/
			typedef String<32>  String32;
			/*********************************************************************
			* Blam::Cache::DataTypes::String64
			* Storing and Manipulating Character Sequences(String<64>)
			**********************************************************************/
			typedef String<64>  String64;
			/*********************************************************************
			* Blam::Cache::DataTypes::String128
			* Storing and Manipulating Character Sequences(String<128>)
			**********************************************************************/
			typedef String<128> String128;
			/*********************************************************************
			* Blam::Cache::DataTypes::String256
			* Storing and Manipulating Character Sequences(String<256>)
			**********************************************************************/
			typedef String<256> String256;
			/*********************************************************************
			* Blam::Cache::DataTypes::String512
			* Storing and Manipulating Character Sequences(String<512>)
			**********************************************************************/
			typedef String<512> String512;
#pragma endregion
#pragma region Unicode

			/*********************************************************************
			* Blam::Cache::DataTypes::Unicode
			* Storing and Manipulating Unicode Character Sequences(Wide String)
			**********************************************************************/
			template<int length>
			struct Unicode
			{
				wchar_t String[length];
				std::wstring GetString();
				void operator = (const char* Value);
				void operator = (const Unicode Value);
			};
			/*********************************************************************
			* Blam::Cache::DataTypes::Unicode32
			* Storing and Manipulating Unicode Character Sequences(Unicode<32>)
			**********************************************************************/
			typedef Unicode<32>  Unicode32;
			/*********************************************************************
			* Blam::Cache::DataTypes::Unicode64
			* Storing and Manipulating Unicode Character Sequences(Unicode<64>)
			**********************************************************************/
			typedef Unicode<64>  Unicode64;
			/*********************************************************************
			* Blam::Cache::DataTypes::Unicode128
			* Storing and Manipulating Unicode Character Sequences(Unicode<128>)
			**********************************************************************/
			typedef Unicode<128> Unicode128;
			/*********************************************************************
			* Blam::Cache::DataTypes::Unicode256
			* Storing and Manipulating Unicode Character Sequences(Unicode<256>)
			**********************************************************************/
			typedef Unicode<256> Unicode256;
#pragma endregion
		}
	}

}

template<int T>
inline std::string  Blam::Cache::DataTypes::String<T>::GetString()
{
	std::string k = this->Text;
	return k;
}
template<int T>
inline void Blam::Cache::DataTypes::String<T>::operator= (const char* Value)
{
	this->Text = Value;
}
template<int T>
inline void Blam::Cache::DataTypes::String<T>::operator= (const String Value)
{
	this->Text = Value.Text;
}

template<int T>
inline std::wstring  Blam::Cache::DataTypes::Unicode<T>::GetString()
{
	std::wstring k = this->Text;
	return k;
}
template<int T>
inline void Blam::Cache::DataTypes::Unicode<T>::operator= (const char* Value)
{
	this->Text = Value;
}
template<int T>
inline void Blam::Cache::DataTypes::Unicode<T>::operator= (const Unicode Value)
{
	this->Text = Value.Text;
}

