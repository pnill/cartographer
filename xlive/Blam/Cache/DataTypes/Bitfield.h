#pragma once
namespace Blam
{
	namespace Cache
	{
		namespace DataTypes
		{
			/*********************************************************************
			* Blam::Cache::DataTypes::Flags
			* Used as a reference to Store All BitFlags in a Bitfield		
			**********************************************************************/
			struct Flags
			{
				Flags(BYTE value) { index = value; }
				inline BYTE GetIndex() { return this->index; }
				inline void operator = (const BYTE Value) { this->index = Value; }
				inline void operator = (const int Value) { this->index = (BYTE)Value; }
			private:
				BYTE index;
			};
			/*********************************************************************
			* Blam::Cache::DataTypes::BitField
			* For Manipulating Bitfield Data aka Flags Field
			* FieldType corresponds to the Size of Bitfield
			* FieldList corresponds to the Structure containing Flags
			**********************************************************************/
			template<typename FieldType, typename FieldList>
			struct BitField
			{
				//Default Constructor
				BitField()
				{
					Handle = 0;
				}
				//Copy Constructor
				BitField(FieldType Value)
				{
					this->Handle = Value;
				}
				//* Returns Bitfield Flag Elements
				FieldList GetElements();
				char GetFlag(Flags Flag);
				void SetFlag(Flags Flag, bool Checked);
				void ToggleFlag(Flags Flag);

				void operator = (const UINT8 Value);
				void operator = (const UINT16 Value);
				void operator = (const UINT32 Value);
				void operator = (const BitField BitField);
				bool operator == (const UINT8 Value);
				bool operator == (const UINT16 Value);
				bool operator == (const UINT32 Value);
				bool operator == (const BitField BitField);
				bool operator != (const UINT8 Value);
				bool operator != (const UINT16 Value);
				bool operator != (const UINT32 Value);
				bool operator != (const BitField BitField);
			private:
				FieldType Handle;
				char GetBit(int index);
				void SetBit(int index, bool state);
				void ToggleBit(int index);
			};

			template<typename FieldList>
			/*********************************************************************
			* Blam::Cache::DataTypes::BitField8
			* For Manipulating 1 BYTE Bitfield Data aka Flags Field
			**********************************************************************/
			using BitField8 = BitField<UINT8, FieldList>;

			template<typename FieldList>
			/*********************************************************************
			* Blam::Cache::DataTypes::BitField16
			* For Manipulating 2 BYTE Bitfield Data aka Flags Field
			**********************************************************************/
			using BitField16 = BitField<UINT16, FieldList>;

			template<typename FieldList>
			/*********************************************************************
			* Blam::Cache::DataTypes::BitField32
			* For Manipulating 4 BYTE Bitfield Data aka Flags Field
			**********************************************************************/
			using BitField32 = BitField<UINT32, FieldList>;

		}
	}

}
#pragma region Bitfield
template<typename FieldType, typename FieldList>
inline  char Blam::Cache::DataTypes::BitField<FieldType, FieldList>::GetBit(int index)
{
	return (this->Value >> index) & 1U; //Shift the bits and return the last one		
}
template<typename FieldType, typename FieldList>
inline  void Blam::Cache::DataTypes::BitField<FieldType, FieldList>::SetBit(int index, bool state)
{
	if (state)
		this->Value = this->Value | (1 << index); //Shift 1 to index bits then OR
	else
		this->Value = this->Value & ~(1 << index); //Shift 1 to index bits then Flip the bits then AND
}
template<typename FieldType, typename FieldList>
inline  void Blam::Cache::DataTypes::BitField<FieldType, FieldList>::ToggleBit(int index)
{
	this->Value = this->Value ^ (1 << index); //Shift 1 to index bits then XOR
}
template<typename FieldType, typename FieldList>
inline FieldList Blam::Cache::DataTypes::BitField<FieldType, FieldList>::GetElements()
{
	return FieldList();
}
template<typename FieldType, typename FieldList>
inline char Blam::Cache::DataTypes::BitField<FieldType, FieldList>::GetFlag(Flags Flag)
{
	return GetBit(Flag.GetIndex());
}
template<typename FieldType, typename FieldList>
inline void Blam::Cache::DataTypes::BitField<FieldType, FieldList>::SetFlag(Flags Flag, bool Value)
{
	SetBit(Flag.GetIndex(), Value);
}
template<typename FieldType, typename FieldList>
inline void Blam::Cache::DataTypes::BitField<FieldType, FieldList>::ToggleFlag(Flags Flag)
{
	ToggleBit(Flag.GetIndex());
}
template<typename FieldType, typename FieldList>
inline void Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator=(const UINT8 Value)
{
	this->Handle = (FieldType)Value;
}
template<typename FieldType, typename FieldList>
inline void Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator=(const UINT16 Value)
{
	this->Handle = (FieldType)Value;
}
template<typename FieldType, typename FieldList>
inline void Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator=(const UINT32 Value)
{
	this->Handle = (FieldType)Value;
}
template<typename FieldType, typename FieldList>
inline void Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator=(const BitField BitField)
{
	this->Handle = BitField;
}
template<typename FieldType, typename FieldList>
inline bool Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator==(const UINT8 Value)
{
	return this->Handle == (FieldType)Value;
}
template<typename FieldType, typename FieldList>
inline bool Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator==(const UINT16 Value)
{
	return this->Handle == (FieldType)Value;
}
template<typename FieldType, typename FieldList>
inline bool Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator==(const UINT32 Value)
{
	return this->Handle == (FieldType)Value;
}
template<typename FieldType, typename FieldList>
inline bool Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator==(const BitField BitField)
{
	return this->Handle == BitField.Handle;
}
template<typename FieldType, typename FieldList>
inline bool Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator!=(const UINT8 Value)
{
	return this->Handle != (FieldType)Value;
}
template<typename FieldType, typename FieldList>
inline bool Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator!=(const UINT16 Value)
{
	return this->Handle != (FieldType)Value;
}
template<typename FieldType, typename FieldList>
inline bool Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator!=(const UINT32 Value)
{
	return this->Handle != (FieldType)Value;
}
template<typename FieldType, typename FieldList>
inline bool Blam::Cache::DataTypes::BitField<FieldType, FieldList>::operator!=(const BitField BitField)
{
	return this->Handle != BitField.Handle;
}
#pragma endregion