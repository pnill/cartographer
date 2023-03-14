#pragma once

#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Math\BlamMath.h"

#pragma pack(push,1)
struct character_tag_group
{
	int CharacterFlags; // pad fuck it.
	//BitField32 CharacterFlags;
	//[TagRefence("char")]
	tag_reference ParentCharacter;
	//[TagRefence("unit")]
	tag_reference Unit;
	//[TagRefence("crea")]
	tag_reference Creature;
	//[TagRefence("styl")]
	tag_reference Style;
	//[TagRefence("char")]
	tag_reference MajorCharacter;
	
	__int64 Variants;
	//Reflexive Variants;
	__int64 GeneralProperties;
	//Reflexive GeneralProperties;
	__int64 VitalityProperties;
	//Reflexive VitalityProperties;
	__int64 PlacementProperties;
	//Reflexive PlacementProperties;
	__int64 PerceptionProperties;
	//Reflexive PerceptionProperties;
	__int64 LookProperties;
	//Reflexive LookProperties;
	__int64 MovementProperties;
	//Reflexive MovementProperties;
	struct character_swarm_block
	{
		/// After the given number of deaths, the swarm scatters
		short scatterKilledCount;
		BYTE padding359[2];
		/// the distance from the target that the swarm scatters
		float scatterRadius;
		/// amount of time to remain scattered
		float scatterTime;
		float houndMinDistance;
		float houndMaxDistance;
		/// amount of randomness added to creature's throttle
		float perlinOffsetScale01;
		/// how fast the creature changes random offset to throttle
		real_vector2d offsetPeriodS;
		/// a random offset lower then given threshold is made 0. (threshold of 1 = no movement)
		float perlinIdleMovementThreshold01;
		/// a random offset lower then given threshold is made 0. (threshold of 1 = no movement)
		float perlinCombatMovementThreshold01;
	};
	tag_block<character_swarm_block> SwarmProperties;
	__int64 ReadyProperties;
	//Reflexive ReadyProperties;
	__int64 EngageProperties;
	//Reflexive EngageProperties;
	__int64 ChargeProperties;
	//Reflexive ChargeProperties;
	__int64 EvasionProperties;
	//Reflexive EvasionProperties;
	__int64 CoverProperties;
	//Reflexive CoverProperties;
	__int64 RetreatProperties;
	//Reflexive RetreatProperties;
	__int64 SearchProperties;
	//Reflexive SearchProperties;
	__int64 PreSearchProperties;
	//Reflexive PreSearchProperties;
	__int64 IdleProperties;
	//Reflexive IdleProperties;
	__int64 VocalizationProperties;
	//Reflexive VocalizationProperties;
	__int64 BoardingProperties;
	//Reflexive BoardingProperties;
	__int64 BossProperties;
	//Reflexive BossProperties;
	__int64 WeaponsProperties;
	//Reflexive WeaponsProperties;
	__int64 FiringPatternProperties;
	//Reflexive FiringPatternProperties;
	__int64 GrenadeProperties;
	//Reflexive GrenadeProperties;
	__int64 VehicleProperties;
	//Reflexive VehicleProperties;
};
#pragma pack(pop)
