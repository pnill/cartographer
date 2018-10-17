#pragma once
#pragma pack(push,1)
#ifndef HUDG_H
#define HUDG_H
#include "Blam\Cache\DataTypes\DataTypes.h"
#include "Blam\Cache\TagBlocks\Globals\hudg.hpp"
#include "Blam\Cache\TagGroups.hpp"
using namespace Blam::Cache;
namespace Blam
{
	namespace Cache
	{
		namespace TagGroups
		{
			namespace Globals
			{
				/*********************************************************************
				* name: hud_globals
				* group_tag: hudg
				* header size: 1160
				**********************************************************************/
				struct hudg :TagGroup<'hudg'>
				{										
					PAD(36);
					Blam::Enums::Tags::TagGroups::hudg::Anchor Anchor;
					Blam::Maths::Integer::Byte::Point2D AnchorOffset;
					PAD(32);
					DataTypes::tagRef Obsolete1;
					DataTypes::tagRef Obsolete2;
					float MessageUpTime;
					float MessageFadeTime;
					Blam::Maths::Real::ColorARGB IconColor;
					Blam::Maths::Real::ColorARGB TextColor;
					float TextSpacing;
					DataTypes::tagRef Itemmessagetext;
					DataTypes::tagRef IconBitmap;
					DataTypes::tagRef AlternateIcon;
					DataTypes::Reflexive<TagBlocks::Globals::button_icons> ButtonIcons;

					struct {

						Blam::Maths::Integer::Byte::ColorARGB HudHelpDefaultColor;
						Blam::Maths::Integer::Byte::ColorARGB HudHelpFlashingColor;
						float HudHelpFlashPeriod;
						float HudHelpFlashDelay;
						INT16 NumberOfFlashes;
						DataTypes::BitField16<Blam::Enums::Tags::TagGroups::hudg::FlashFlags> HudHelpFlashFlags;
						float HudHelpFlashLength;
						Blam::Maths::Integer::Byte::ColorARGB DisabledColor;
						Blam::Maths::Integer::Byte::ColorARGB DisabledColor2;
						DataTypes::tagRef Hudmessages;
					}HudHelp;

					struct {
						Blam::Maths::Integer::Byte::ColorARGB ObjectiveDefaultColor;
						Blam::Maths::Integer::Byte::ColorARGB ObjectiveFlashing;
						float ObjectiveFlashPeriod;
						float ObjectiveFlashDelay;
						INT16 NumberOfFlashes;
						DataTypes::BitField16<Blam::Enums::Tags::TagGroups::hudg::FlashFlags> ObjectiveFlashFlags;
						float ObjectiveFlashLength;
						Blam::Maths::Integer::Byte::ColorARGB DisabledColor;
						INT16 ObjectiveUptimeTicks;
						INT16 ObjectiveFadeTicks;
					}Objective;

					float WaypointTopOffset;
					float WaypointBottomOffset;
					float WaypointLeftOffset;
					float WaypointRightOffset;
					PAD(32);
					DataTypes::tagRef Arrowbitmap;
					DataTypes::Reflexive<TagBlocks::Globals::waypoints_arrows>	WaypointsArrows;
					PAD(80);
					float MultiplayerHudScale;
					PAD(272);
					float MotionSensorRange;
					float MotionSensorVelocitySensitivity;
					float MotionSensorScale;
					INT16 DefaultTitleChapterBoundstop;
					INT16 DefaultTitleChapterBoundsleft;
					INT16 DefaultTitleChapterBoundsbottom;
					INT16 DefaultTitleChapterBoundsright;
					PAD(44);

					INT16 HudDamageIndicatorOffsettop;
					INT16 HudDamageIndicatorOffsetbottom;
					INT16 HudDamageIndicatorOffsetleft;
					INT16 HudDamageIndicatorOffsetright;
					PAD(32);
					DataTypes::tagRef DamageIndicatorBitmap;
					INT16 SequenceIndex;
					INT16 MultiplayerSequenceIndex;
					Blam::Maths::Integer::Byte::ColorARGB DamageIndicatorColor;
					PAD(16);

					Blam::Maths::Integer::Byte::ColorARGB NotMuchTimeLeftDefaultColor;
					Blam::Maths::Integer::Byte::ColorARGB NotMuchTimeLeftFlashColor;
					float NotMuchTimeLeftFlashPeriod;
					float NotMuchTimeLeftFlashDelay;
					INT16 NumberOfFlashes;
					DataTypes::BitField16<Blam::Enums::Tags::TagGroups::hudg::FlashFlags> NotMuchTimeLeftFlashFlags;
					float NotMuchTimeLeftFlashLength;
					PAD(8);

					struct
					{
						Blam::Maths::Integer::Byte::ColorARGB TimeOutDefaultColor;
						Blam::Maths::Integer::Byte::ColorARGB TimeOutFlashingColor;
						float TimeOutFlashPeriod;
						float TimeOutFlashDelay;
						INT16 NumberOfFlashes;
						DataTypes::BitField16<Blam::Enums::Tags::TagGroups::hudg::FlashFlags>	TimeOutFlashFlags;
						float TimeOutFlashLength;
						Blam::Maths::Integer::Byte::ColorARGB DisabledTimeOutDefaultColor;
					}Timeout;

					PAD(44);
					DataTypes::tagRef CarnageReportBitmap;
					INT16 LoadingBeginText;
					INT16 LoadingEndText;
					INT16 CheckpointBeginText;
					INT16 CheckpointEndText;
					DataTypes::tagRef CheckpointSound;
					PAD(96);
					struct
					{
						DataTypes::tagRef Hudtext;
						DataTypes::Reflexive<TagBlocks::Globals::dashlights> Dashlights;
						DataTypes::Reflexive<TagBlocks::Globals::waypoint_arrows> WaypointArrows;
						DataTypes::Reflexive<TagBlocks::Globals::waypoints_arrows> WayPoints;
						DataTypes::Reflexive<TagBlocks::Globals::hud_sounds> HudSounds;
						DataTypes::Reflexive<TagBlocks::Globals::player_training_data> PlayerTrainingData;
						DataTypes::tagRef PrimaryMessageSound;
						DataTypes::tagRef SecondaryMessageSound;
						DataTypes::StringID BootGrieferString;
						DataTypes::StringID CannotBootGrieferString;
						DataTypes::tagRef TrainingShader;
						DataTypes::tagRef HumanTrainingTopRight;
						DataTypes::tagRef HumanTrainingTopCenter;
						DataTypes::tagRef HumanTrainingTopLeft;
						DataTypes::tagRef HumanTrainingcenter;
						DataTypes::tagRef EliteTrainingTopRight;
						DataTypes::tagRef EliteTrainingTopCenter;
						DataTypes::tagRef EliteTrainingTopLeft;
						DataTypes::tagRef EliteTrainingCenter;
					}NewHudGlobals;
					
				};				
				TAG_GROUP_SIZE_ASSERT(hudg, 1160);

			}
		}
	}
}
#endif
#pragma pack(pop)