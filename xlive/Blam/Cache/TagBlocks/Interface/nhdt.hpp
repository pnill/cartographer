#pragma once
#pragma pack(push,1)
#ifndef NHDT_H_B
#define NHDT_H_B
#include "Blam\Cache\DataTypes\DataTypes.h"
#include "Blam\Cache\TagBlocks.hpp"
#include "Blam\Maths\Maths.h"
#include "Blam\Enums\Tags\TagGroups\nhdt.hpp"

using namespace Blam::Cache;

namespace Blam
{
	namespace Cache
	{
		namespace TagBlocks
		{
			namespace Interface
			{
				/****************************************
				* size: 104
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct effect :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::newhuddefinition>
				{
					DataTypes::BitField32<Blam::Enums::Tags::TagGroups::nhdt::Effect::Flags> Flags;
					struct Effects
					{
						DataTypes::StringID InputName;
						DataTypes::StringID RangeName;
						float TimePeriod;
						DataTypes::dataRef Function;
					};
					Effects Data1;
					Effects Data2;
					Effects Data3;
					Effects Data4;
					Effects Data5;
				};
				TAG_BLOCK_SIZE_ASSERT(effect, 104);
				/****************************************
				* size: 100
				* alignment:    4
				* max_count:    256
				*
				****************************************/
				struct bitmap_widgets :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::newhuddefinition>
				{
					DataTypes::StringID Name;
					Blam::Enums::Tags::TagGroups::nhdt::Input Input1;
					Blam::Enums::Tags::TagGroups::nhdt::Input Input2;
					Blam::Enums::Tags::TagGroups::nhdt::Input Input3;
					Blam::Enums::Tags::TagGroups::nhdt::Input Input4;

					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfUnitFlags> DoDrawIfUnitFlags;
					DataTypes::BitField8<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfExtraFlags>  DoDrawIfExtraFlags;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfWeaponFlags> DoDrawIfWeaponFlags;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfGameEngineStateFlags> DoDrawIfGameEngineStateFlags;

					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfUnitFlags> DontDrawIfUnitFlags;
					DataTypes::BitField8<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfExtraFlags>  DontDrawIfExtraFlags;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfWeaponFlags> DontDrawIfWeaponFlags;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfGameEngineStateFlags> DontDrawIfGameEngineStateFlags;
					INT16 AgeCutoff;
					INT16 ClipCutoff;
					INT16 TotalCutoff;

					Blam::Enums::Tags::TagGroups::nhdt::Anchor Anchor;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::Flags> Flags;
					DataTypes::tagRef Bitmap;
					DataTypes::tagRef Shader;
					BYTE FullscreenSequenceIndex;
					BYTE HalfscreenSequenceIndex;
					BYTE QuarterscreenSequenceIndex;
					PAD8;
					Maths::Integer::Short::Point2D FullscreenOffset;
					Maths::Integer::Short::Point2D HalfscreenOffset;
					Maths::Integer::Short::Point2D QuarterscreenOffset;
					Maths::Real::Point2D FullscreenRegistrationPoint;
					Maths::Real::Point2D HalfscreenRegistrationPoint;
					Maths::Real::Point2D QuarterscreenRegistrationPoint;
					DataTypes::Reflexive<effect> EffectBlock;
					Blam::Enums::Tags::TagGroups::nhdt::SpecialHUDType SpecialHUDType;
				};
				TAG_BLOCK_SIZE_ASSERT(bitmap_widgets, 100);

				/****************************************
				* size: 84
				* alignment:    4
				* max_count:    256
				*
				****************************************/
				struct numerical_Elements :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::newhuddefinition>
				{
					DataTypes::StringID Name;
					Blam::Enums::Tags::TagGroups::nhdt::Input Input1;
					Blam::Enums::Tags::TagGroups::nhdt::Input Input2;
					Blam::Enums::Tags::TagGroups::nhdt::Input Input3;
					Blam::Enums::Tags::TagGroups::nhdt::Input Input4;

					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfUnitFlags> DoDrawIfUnitFlags;
					DataTypes::BitField8<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfExtraFlags>  DoDrawIfExtraFlags;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfWeaponFlags> DoDrawIfWeaponFlags;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfGameEngineStateFlags> DoDrawIfGameEngineStateFlags;

					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfUnitFlags> DontDrawIfUnitFlags;
					DataTypes::BitField8<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfExtraFlags>  DontDrawIfExtraFlags;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfWeaponFlags> DontDrawIfWeaponFlags;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::DoDrawIfGameEngineStateFlags> DontDrawIfGameEngineStateFlags;

					INT16 AgeCutoff;
					INT16 ClipCutoff;
					INT16 TotalCutoff;

					Blam::Enums::Tags::TagGroups::nhdt::Anchor Anchor;
				    DataTypes::BitField16<Blam::Enums::Tags::TagGroups::nhdt::NumericalElements::Flags> Flags;
					
					DataTypes::tagRef Shader;
					DataTypes::StringID StringName;
					Blam::Enums::Tags::TagGroups::nhdt::Justification Justification;
					Blam::Enums::Tags::TagGroups::nhdt::ScreenFontIndex FullscreenFontIndex;
					Blam::Enums::Tags::TagGroups::nhdt::ScreenFontIndex HalfscreenFontIndex;
					Blam::Enums::Tags::TagGroups::nhdt::ScreenFontIndex QuarterscreenFontIndex;
					float FullscreenScale;
					float HalfscreenScale;
					float QuarterscreenScale;
					Maths::Integer::Short::Point2D FullscreenOffset;
					Maths::Integer::Short::Point2D HalfscreenOffset;
					Maths::Integer::Short::Point2D QuarterscreenOffset;				
					DataTypes::Reflexive<effect> EffectBlock;
				};
				TAG_BLOCK_SIZE_ASSERT(numerical_Elements, 84);

			}
		}
	}
}

#endif
#pragma pack(pop)