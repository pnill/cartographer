#include "Blam/Math/real_math.h"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"

struct s_weapon_custom_offset {
	const real_vector3d DefaultOffset;
	const char WeaponPath[128];
	real_vector3d ModifiedOffset;
	s_weapon_group_definition* tag;
};

enum Weapons
{
	BattleRifle,
	BeamRifle,
	BrutePlasmaRifle,
	BruteShot,
	Carbine,
	EnergySword,
	FuelRod,
	Magnum,
	Needler,
	PlasmaPistol,
	PlasmaRifle,
	RocketLauncher,
	SentinelBeam,
	Shotgun,
	SMG,
	Sniper
};

void ReadWeaponOffsetConfig(s_weapon_custom_offset* WeaponOffsets);
void SaveWeaponOffsetConfig(const s_weapon_custom_offset customOffsets[], bool defaultOffsets);
void WriteDefaultFile(const s_weapon_custom_offset WeaponOffsets[]);

