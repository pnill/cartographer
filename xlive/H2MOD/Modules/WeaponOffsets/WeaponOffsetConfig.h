
#include "items/weapon_definitions.h"

struct s_weapon_custom_offset 
{
	const real_point3d defaultOffset;
	const char weaponPath[128];
	real_point3d modifiedOffset;
	_weapon_definition* tag;
};

void ReadWeaponOffsetConfig(s_weapon_custom_offset* weaponOffsets, int count);
void SaveWeaponOffsetConfig(const s_weapon_custom_offset* customOffsets, int count, bool defaultOffsets);
void WriteDefaultFile(const s_weapon_custom_offset* weaponOffsets, int count);
