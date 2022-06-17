#include "stdafx.h"

#include "WeaponOffsetConfig.h"

static std::wstring GetOffsetPath()
{
	std::wstring offsetPath;
	offsetPath = _wgetenv(L"localappdata");
	offsetPath += L"\\Microsoft\\Halo 2\\WeaponOffsets.cfg";
	return offsetPath;
}

void ReadWeaponOffsetConfig(s_weapon_custom_offset* weaponOffsets, int count)
{
	FILE* file;
	std::wstring path = GetOffsetPath();

	file = _wfopen(path.c_str(), L"r");
	if (file != NULL)
	{
		for (int i = 0; i < count; i++)
		{
			fscanf(file, "%f,%f,%f\n", &weaponOffsets[i].modifiedOffset.i, &weaponOffsets[i].modifiedOffset.j, &weaponOffsets[i].modifiedOffset.k);
		}
		fclose(file);
	}
}

void SaveWeaponOffsetConfig(const s_weapon_custom_offset* customOffsets, int count, bool defaultOffsets)
{
	FILE* file;
	std::wstring path = GetOffsetPath();

	file = _wfopen(path.c_str(), L"w");
	if (file != NULL)
	{
		for (int i = 0; i < count; i++)
		{
			const real_vector3d& offset = defaultOffsets ? customOffsets[i].defaultOffset : customOffsets[i].modifiedOffset;
			fprintf(file, "%.3f,%.3f,%.3f\n", offset.i, offset.j, offset.k);
		}
		fclose(file);
	}
}

// only writes anything if file dosent already exist
void WriteDefaultFile(const s_weapon_custom_offset* weaponOffsets, int count)
{
	FILE* file = NULL;
	std::wstring path = GetOffsetPath();

	file = _wfopen(path.c_str(), L"r");
	if (file == NULL)
	{
		SaveWeaponOffsetConfig(weaponOffsets, count, true);
	}
	else
	{
		fclose(file);
	}
}
