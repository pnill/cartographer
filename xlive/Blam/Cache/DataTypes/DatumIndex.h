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
					this->Index = 0xFFFF;
					this->Salt = 0xFFFF;
				}
				//Copy Constructor
				DatumIndex(INT32 Identifier)
				{
					this->Index = Identifier & 0x0000FFFF;
					this->Salt = Identifier & 0xFFFF0000;
				}
				static const INT32 Null = 0xFFFFFFFF;
			public:
				//Returns Absolute Index
				INT16 ToAbsoluteIndex();
				//Returns Datum Salt
				INT16 ToIdentifier();
				//Returns true is DatumIndex is not Invalid
				bool IsNull();
				//Return in StringFormat
				std::string ToString();

				void operator = (const INT32 &Value);
				void operator = (const DatumIndex &DatumIndex);
				bool operator == (const INT32 &Value);
				bool operator == (const DatumIndex &DatumIndex);
				bool operator != (const INT32 &Value);
				bool operator != (const DatumIndex &DatumIndex);

				INT16 Index;
			private:
				//Absolute Index 

				//Salt for Unique Identification
				INT16 Salt;

			};
			static_assert(sizeof(DatumIndex) == 4, "Invalid Size for struct (DatumIndex)");
		}
	}
}
			
#pragma region DatumIndex
inline INT16 Blam::Cache::DataTypes::DatumIndex::ToAbsoluteIndex()
{
	return this->Index;
}
inline INT16 Blam::Cache::DataTypes::DatumIndex::ToIdentifier()
{
	return this->Salt;
}
inline bool Blam::Cache::DataTypes::DatumIndex::IsNull()
{

	return this->Salt = 0xFFFF && this->Index == 0xFFFF;

}
inline void Blam::Cache::DataTypes::DatumIndex::operator = (const INT32 &Value)
{
	this->Index = Value & 0x0000FFFF;
	this->Salt = Value & 0xFFFF0000;
}
inline void Blam::Cache::DataTypes::DatumIndex::operator = (const DatumIndex &DatumIndex)
{
	this->Index = DatumIndex.Index;
	this->Salt = DatumIndex.Salt;
}
inline bool Blam::Cache::DataTypes::DatumIndex::operator== (const INT32 &Value)
{
	return (this->Index == Value & 0x0000FFFF) && (this->Salt == Value & 0xFFFF0000);
}
inline bool Blam::Cache::DataTypes::DatumIndex::operator== (const DatumIndex &DatumIndex)
{
	return (this->Index == DatumIndex.Index) && (this->Salt == DatumIndex.Salt);
}
inline bool Blam::Cache::DataTypes::DatumIndex::operator!= (const INT32 &Value)
{
	return (this->Index != Value & 0x0000FFFF) || (this->Salt != Value & 0xFFFF0000);
}
inline bool Blam::Cache::DataTypes::DatumIndex::operator!= (const DatumIndex &DatumIndex)
{
	return (this->Index != DatumIndex.Index) || (this->Salt != DatumIndex.Salt);
}
inline std::string Blam::Cache::DataTypes::DatumIndex::ToString()
{
	std::string val;
	val = (UINT32)((this->Salt << 16) | this->Index);
	return val;
}

#pragma endregion