#include "stdafx.h"

#include <shlobj.h>
#include "WeaponOffsetConfig.h"

wchar_t* GetOffsetPath()
{
    wchar_t* path;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &path);
    return wcsncat(path, L"\\Microsoft\\Halo 2\\WeaponOffsets.cfg", MAX_PATH);
}

void ReadWeaponOffsetConfig(s_Weapon_custom_offset *WeaponOffsets, const byte size)
{
    FILE* file = NULL;
    wchar_t *path = GetOffsetPath();

    file = _wfopen(path, L"r");
    if (file != NULL)
    {
        for (byte i = 0; i < size; i++)
        {
            fscanf(file, "%f,%f,%f\n", &WeaponOffsets[i].modifiedOffset.i, &WeaponOffsets[i].modifiedOffset.j, &WeaponOffsets[i].modifiedOffset.k);
        }
        fclose(file);
    }
}

void SaveWeaponOffsetConfig(const s_Weapon_custom_offset *customOffsets, const byte size, bool defaultOffsets)
{
    FILE* file = NULL;
    wchar_t* path = GetOffsetPath();

    file = _wfopen(path, L"w");
    if (file != NULL)
    {
        if (defaultOffsets == false)
        {
            for (byte i = 0; i < size; i++)
            {
                fprintf(file, "%.3f,%.3f,%.3f\n", customOffsets[i].modifiedOffset.i, customOffsets[i].modifiedOffset.j, customOffsets[i].modifiedOffset.k);
            }
        }
        else
        {
            for (byte i = 0; i < size; i++)
            {
                fprintf(file, "%.3f,%.3f,%.3f\n", customOffsets[i].defaultOffset.i, customOffsets[i].defaultOffset.j, customOffsets[i].defaultOffset.k);
            }
        }
        fclose(file);
    }
}

// only writes anything if file dosent already exist
void WriteDefaultFile(const s_Weapon_custom_offset *WeaponOffsets, const byte size)
{
    FILE* file = NULL;
    wchar_t* path = GetOffsetPath();

    file = _wfopen(path, L"r");

    if (file == NULL)
    {
        SaveWeaponOffsetConfig(WeaponOffsets, size, true);
    }
    else fclose(file);
}
