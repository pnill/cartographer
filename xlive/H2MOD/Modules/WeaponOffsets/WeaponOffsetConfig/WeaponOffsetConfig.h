#include "Blam/Math/real_math.h"

void ReadWeaponOffsetConfig(real_vector3d *WeaponOffset);
void SaveWeaponOffsetConfig(const real_vector3d WeaponOffset[]);
void WriteDefaultFile(const real_vector3d WeaponOffset[]);

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