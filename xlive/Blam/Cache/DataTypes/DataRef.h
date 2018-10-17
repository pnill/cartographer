#pragma once
namespace Blam
{
	namespace Cache
	{
		namespace DataTypes
		{
			/*********************************************************************
			* Blam::Cache::DataTypes::dataRef
			* 8 BYTE Tag Structure for any Data Block Field
			**********************************************************************/
			struct dataRef
			{
				dataRef(INT32 BlockSize)
				{

				}
				dataRef(INT32 BlockSize, INT32 BlockPtr)
				{

				}
			private:
				INT32 DataSize;
				INT32 DataOffset;
				const char* GetBytes();
				const wchar_t* ToUnicode();
				void DeleteData();
			};
			static_assert(sizeof(dataRef) == 8, "Invalid Size for struct (dataRef)");
		}
	}
}