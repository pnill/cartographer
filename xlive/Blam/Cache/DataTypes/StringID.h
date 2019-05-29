#pragma once
#include <codecvt>
namespace Blam
{
	namespace Cache
	{
		namespace DataTypes
		{			
			/*********************************************************************
			* Blam::Cache::DataTypes::StringID
			* 4 BYTE Special Tag Structure for String Values
			* Index -3 Bytes
			* Length -1 Byte
			**********************************************************************/
			struct StringID
			{
				StringID()
				{
					this->Handle = Empty;
				}
				StringID(UINT32 Value)
				{
					this->Handle = Empty;
					this->Handle = Value;
				}
				StringID(UINT Index, BYTE Length)
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
				void operator = (const StringID &StringID);
				bool operator == (const UINT32 &Value);
				bool operator == (const StringID &StringID);
				bool operator != (const UINT32 &Value);
				bool operator != (const StringID &StringID);
			private:
				UINT32 Handle;

			};
			static_assert(sizeof(StringID) == 4, "Invalid Size for struct (StringID)");
		}
	}

}

#pragma region StringID
inline bool Blam::Cache::DataTypes::StringID::IsEmpty()
{
	return this->Handle == Empty;
}
inline bool Blam::Cache::DataTypes::StringID::IsInvalid()
{
	return this->Handle == Invalid;
}
inline UINT Blam::Cache::DataTypes::StringID::ToIndex()
{
	return this->Handle & 0x00FFFFFF;
}
inline BYTE Blam::Cache::DataTypes::StringID::ToLength()
{
	return (BYTE)(this->Handle & 0xFF000000);
}
inline void Blam::Cache::DataTypes::StringID::operator= (const UINT32 &Value)
{
	this->Handle = Value;
}
inline void Blam::Cache::DataTypes::StringID::operator=(const StringID &StringID)
{
	this->Handle = StringID.Handle;
}
inline bool Blam::Cache::DataTypes::StringID::operator== (const UINT32 &Value)
{
	return this->Handle == Value;
}
inline bool Blam::Cache::DataTypes::StringID::operator== (const StringID &StringID)
{
	return this->Handle == StringID.Handle;
}
inline bool Blam::Cache::DataTypes::StringID::operator!= (const UINT32 &Value)
{
	return this->Handle != Value;
}
inline bool Blam::Cache::DataTypes::StringID::operator!= (const StringID &StringID)
{
	return this->Handle != StringID.Handle;
}
inline std::string Blam::Cache::DataTypes::StringID::ToString()
{
	std::string val;
	val = this->Handle;
	return val;
}
#pragma endregion