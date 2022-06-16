#include "Blam/Math/real_math.h"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"

struct s_Weapon_custom_offset {
	const real_vector3d defaultOffset;
	const char weaponPath[128];
	real_vector3d modifiedOffset;
	s_weapon_group_definition* tag;
};


wchar_t* GetOffsetPath();
void ReadWeaponOffsetConfig(s_Weapon_custom_offset *WeaponOffsets, const byte size);
void SaveWeaponOffsetConfig(const s_Weapon_custom_offset *customOffsets, const byte size, bool defaultOffsets);
void WriteDefaultFile(const s_Weapon_custom_offset *WeaponOffsets, const byte size);
