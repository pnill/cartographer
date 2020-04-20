#pragma once
#ifndef CHAR_BLOCK_H
#define CHAR_BLOCK_H
#include "Blam\Maths\Maths.h"


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

#endif