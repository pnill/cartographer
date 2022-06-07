#include "stdafx.h"
#include "WeaponOffsetConfig.h"

char* path = strcat
(
    getenv("localappdata"),
    "\\Microsoft\\Halo 2\\WeaponOffsets.cfg"
);

void ReadWeaponOffsetConfig(s_weapon_custom_offset *WeaponOffsets)
{
    FILE* file = NULL;

    file = fopen(path, "r");
    if (file != NULL)
    {
        for (byte i = BattleRifle; i != Sniper + 1; i++)
        {
            fscanf(file, "%f,%f,%f\n", &WeaponOffsets[i].ModifiedOffset.i, &WeaponOffsets[i].ModifiedOffset.j, &WeaponOffsets[i].ModifiedOffset.k);
        }
        fclose(file);
    }
}

void SaveWeaponOffsetConfig(const s_weapon_custom_offset customOffsets[], bool defaultOffsets)
{
    FILE* file = NULL;

    file = fopen(path, "w");
    if (file != NULL)
    {
        if (defaultOffsets == false)
        {
            for (byte i = BattleRifle; i != Sniper + 1; i++)
            {
                fprintf(file, "%.3f,%.3f,%.3f\n", customOffsets[i].ModifiedOffset.i, customOffsets[i].ModifiedOffset.j, customOffsets[i].ModifiedOffset.k);
            }
        }
        else
        {
            for (byte i = BattleRifle; i != Sniper + 1; i++)
            {
                fprintf(file, "%.3f,%.3f,%.3f\n", customOffsets[i].DefaultOffset.i, customOffsets[i].DefaultOffset.j, customOffsets[i].DefaultOffset.k);
            }
        }
        fclose(file);
    }
}

// only writes anything if file dosent already exist
void WriteDefaultFile(const s_weapon_custom_offset WeaponOffsets[])
{
    FILE* file = NULL;    

    file = fopen(path, "r");
    if (file == NULL)
    {
        SaveWeaponOffsetConfig(WeaponOffsets, true);
    }
    else fclose(file);
}