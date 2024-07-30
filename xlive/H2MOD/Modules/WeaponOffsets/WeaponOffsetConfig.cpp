#include "stdafx.h"
#include "WeaponOffsetConfig.h"

#include "cseries/cseries_strings.h"

/* prototypes */
void weapon_offset_get_path(c_static_wchar_string<MAX_PATH>* path);

/* public code */

void ReadWeaponOffsetConfig(s_weapon_custom_offset* weaponOffsets, int count)
{
	c_static_wchar_string<MAX_PATH> path;
	weapon_offset_get_path(&path);

	FILE* file = _wfopen(path.get_string(), L"r");
	if (file != NULL)
	{
		for (int i = 0; i < count; i++)
		{
			fscanf_s(file, "%f,%f,%f\n", &weaponOffsets[i].modifiedOffset.x, &weaponOffsets[i].modifiedOffset.y, &weaponOffsets[i].modifiedOffset.z);
		}
		fclose(file);
	}
	return;
}

void SaveWeaponOffsetConfig(const s_weapon_custom_offset* customOffsets, int count, bool defaultOffsets)
{
	c_static_wchar_string<MAX_PATH> path;
	weapon_offset_get_path(&path);

	FILE* file = _wfopen(path.get_string(), L"w");
	if (file != NULL)
	{
		for (int i = 0; i < count; i++)
		{
			const real_point3d* offset = defaultOffsets ? &customOffsets[i].defaultOffset : &customOffsets[i].modifiedOffset;
			fprintf_s(file, "%.3f,%.3f,%.3f\n", offset->x, offset->y, offset->z);
		}
		fclose(file);
	}
	return;
}

// only writes anything if file dosent already exist
void WriteDefaultFile(const s_weapon_custom_offset* weaponOffsets, int count)
{
	c_static_wchar_string<MAX_PATH> path;
	weapon_offset_get_path(&path);

	FILE* file = _wfopen(path.get_string(), L"r");
	if (file == NULL)
	{
		SaveWeaponOffsetConfig(weaponOffsets, count, true);
	}
	else
	{
		fclose(file);
	}
	return;
}

/* private code */

void weapon_offset_get_path(c_static_wchar_string<MAX_PATH>* path)
{
	path->set(_wgetenv(L"localappdata"));
	path->append(L"\\Microsoft\\Halo 2\\WeaponOffsets.cfg");
	return;
}