#pragma once
#include <Windows.h>
#include <codecvt>
namespace Blam
{
	namespace Cache
	{
		namespace DataTypes
		{
			/*********************************************************************
			* Blam::Cache::DataTypes::DatumIndex
			* 4 BYTE Tag Structure for any TagIndices of Halo 2 CacheFiles
			**********************************************************************/
			struct DatumIndex
			{
				//Default Constructor
				DatumIndex()
				{
					this->Index = 0xFFFFu;
					this->Salt = 0xFFFFu;
				}
				//Copy Constructor
				DatumIndex(INT32 Identifier)
				{
					this->Index = (Identifier & 0x0000FFFF);
					this->Salt = (Identifier & 0xFFFF0000);
				}
				static const INT32 Null = 0xFFFFFFFF;
			public:
				//Returns Absolute Index
				short ToAbsoluteIndex();
				//Returns Datum Salt
				short ToIdentifier();
				//Returns true is DatumIndex is not Invalid
				bool IsNull();
				//Return in StringFormat
				std::string ToString();
				INT ToInt();

				void operator = (const short &Value);
				void operator = (const DatumIndex &DatumIndex);
				bool operator == (const short &Value);
				bool operator == (const DatumIndex &DatumIndex);
				bool operator != (const short &Value);
				bool operator != (const DatumIndex &DatumIndex);

				signed short Index;
			private:
				//Absolute Index 

				//Salt for Unique Identification
				signed short Salt;

			};
			static_assert(sizeof(DatumIndex) == 4, "Invalid Size for struct (DatumIndex)");
		}
	}
}
			
#pragma region DatumIndex
inline INT Blam::Cache::DataTypes::DatumIndex::ToInt()
{
	int datum = ((this->Salt << 16) | ((this->Index) & 0xffff));
	return datum;
}

inline short Blam::Cache::DataTypes::DatumIndex::ToAbsoluteIndex()
{
	return this->Index;
}
inline short Blam::Cache::DataTypes::DatumIndex::ToIdentifier()
{
	return this->Salt;
}
inline bool Blam::Cache::DataTypes::DatumIndex::IsNull()
{

	return this->Salt = 0xFFFF && this->Index == 0xFFFF;

}
inline void Blam::Cache::DataTypes::DatumIndex::operator = (const short &Value)
{
	this->Index = (Value & 0x0000FFFF);
	this->Salt = (Value & 0xFFFF0000);
}
inline void Blam::Cache::DataTypes::DatumIndex::operator = (const DatumIndex &DatumIndex)
{
	this->Index = DatumIndex.Index;
	this->Salt = DatumIndex.Salt;
}
inline bool Blam::Cache::DataTypes::DatumIndex::operator== (const short &Value)
{
	return (this->Index == (Value & 0x0000FFFF)) && (this->Salt == (Value & 0xFFFF0000));
}
inline bool Blam::Cache::DataTypes::DatumIndex::operator== (const DatumIndex &DatumIndex)
{
	return (this->Index == DatumIndex.Index) && (this->Salt == DatumIndex.Salt);
}
inline bool Blam::Cache::DataTypes::DatumIndex::operator!= (const short &Value)
{
	return (this->Index != (Value & 0x0000FFFF)) || (this->Salt != (Value & 0xFFFF0000));
}
inline bool Blam::Cache::DataTypes::DatumIndex::operator!= (const DatumIndex &DatumIndex)
{
	return (this->Index != DatumIndex.Index) || (this->Salt != DatumIndex.Salt);
}
inline std::string Blam::Cache::DataTypes::DatumIndex::ToString()
{
	std::string val;
	val = (INT32)((this->Salt << 16) | this->Index);
	return val;
}

#pragma endregion