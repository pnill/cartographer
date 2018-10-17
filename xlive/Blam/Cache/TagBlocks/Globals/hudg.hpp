#pragma once
#pragma pack(push,1)
#ifndef HUDG_H_B
#define HUDG_H_B
#include "Blam\Cache\DataTypes\DataTypes.h"
#include "Blam\Cache\TagBlocks.hpp"
#include "Blam\Maths\Maths.h"
#include "Blam\Enums\Tags\TagGroups\hudg.hpp"

using namespace Blam::Cache;

namespace Blam
{
	namespace Cache
	{
		namespace TagBlocks
		{
			namespace Globals
			{
				/****************************************
				* size: 16
				* alignment:    4
				* max_count:    18
				*
				****************************************/
				struct button_icons :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::hudglobals>
				{
					INT16 sequenceIndex;
					INT16 widthOffset;
					INT16 offsetFromReferenceCornerx;
					INT16 offsetFromReferenceCornery;
					UINT8 overrideIconColoralpha;
					UINT8 overrideIconColorred;
					UINT8 overrideIconColorgreeb;
					UINT8 overrideIconColorblue;
					UINT8 frameRate030;
					DataTypes::BitField8<Blam::Enums::Tags::TagGroups::hudg::ButtonIcons::ButtonFlags> buttonFlags;
					INT16 textIndex;
				};				
				TAG_BLOCK_SIZE_ASSERT(button_icons, 16);
				/****************************************
				* size: 28
				* alignment:    4
				* max_count:    -1
				*
				****************************************/
				struct dashlights :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::hudglobals>
				{
					DataTypes::tagRef bitmap;
					DataTypes::tagRef shader;
					INT16 sequenceIndex;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::hudg::Dashlights::FlashFlags> timeOutFlashFlags;
					DataTypes::tagRef sound;
				};
				TAG_BLOCK_SIZE_ASSERT(dashlights, 28);				
				/****************************************
				* size: 24
				* alignment:    4
				* max_count:    -1
				*
				****************************************/
				struct hud_sounds :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::hudglobals>
				{
					DataTypes::tagRef chiefsound;
					DataTypes::BitField32<Blam::Enums::Tags::TagGroups::hudg::HudSounds::HudSoundsFlags> HudSoundsFlags;
					float scale;
					DataTypes::tagRef dervishsound;
				};
				TAG_BLOCK_SIZE_ASSERT(hud_sounds, 24);				
				/****************************************
				* size: 20
				* alignment:    4
				* max_count:    -1
				*
				****************************************/
				struct player_training_data :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::hudglobals>
				{
					DataTypes::StringID displayString;
					DataTypes::StringID displayString2;
					DataTypes::StringID displayString3;
					float maxDisplayTime;
					UINT8 displayCount;
					UINT8 disapearDelay;
					UINT8 redisplayDelay;
					UINT8 displayDelays;
				};
				TAG_BLOCK_SIZE_ASSERT(player_training_data, 20);				
				/****************************************
				* size: 36
				* alignment:    4
				* max_count:    -1
				*
				****************************************/
				struct waypoint_arrows :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::hudglobals>
				{
					DataTypes::tagRef bitmap;
					DataTypes::tagRef shader;
					float sequenceIndex;
					float smallestSize;
					float smallestDistance;
					DataTypes::tagRef borderBitmap;
				};
				TAG_BLOCK_SIZE_ASSERT(waypoint_arrows, 36);				
				/****************************************
				* size: 24
				* alignment:    4
				* max_count:    -1
				*
				****************************************/
				struct waypoints :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::hudglobals>
				{
					DataTypes::tagRef bitmap;
					DataTypes::tagRef sahder;
					INT16 onscreenSequenceIndex;
					INT16 occludedSequenceIndex;
					INT16 offscreenSequenceIndex;
					INT16 unused;
				};
				TAG_BLOCK_SIZE_ASSERT(waypoints, 24);
				/****************************************
				* size: 104
				* alignment:    4
				* max_count:    16
				*
				****************************************/
				struct waypoints_arrows :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::hudglobals>
				{
					DataTypes::String32 name;
					PAD(8);
					Blam::Maths::Integer::Byte::ColorRGB ColorRGB;
					BYTE opacity;
					float translucency;
					PAD(4);
					INT16 onScreenSequenceIndex;
					INT16 offScreenSequenceIndex;
					INT16 unusedSequenceIndex;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::hudg::WaypointsArrows::waypoints_arrowsFlags> waypoints_arrowsFlags;
					PAD(44);
				};
				TAG_BLOCK_SIZE_ASSERT(waypoints_arrows, 104);				
			}
		}
	}
}
#pragma pack(pop)
#endif

