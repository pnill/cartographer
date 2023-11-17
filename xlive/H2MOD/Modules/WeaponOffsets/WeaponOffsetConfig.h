
#include "Blam/Engine/math/real_math.h"
#include "Blam/Cache/TagGroups/weapon_definition.hpp"

struct s_weapon_custom_offset {
	const real_vector3d defaultOffset;
	const char weaponPath[128];
	real_vector3d modifiedOffset;
	s_weapon_group_definition* tag;
};

std::wstring GetOffsetPath();
void ReadWeaponOffsetConfig(s_weapon_custom_offset* weaponOffsets, int count);
void SaveWeaponOffsetConfig(const s_weapon_custom_offset* customOffsets, int count, bool defaultOffsets);
void WriteDefaultFile(const s_weapon_custom_offset* weaponOffsets, int count);
