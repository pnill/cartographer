#include "stdafx.h"
#include "Blam/Math/real_math.h"
#include "WeaponOffsetConfig.h"

char* path = strcat
(
    getenv("localappdata"),
    "\\Microsoft\\Halo 2\\WeaponOffsets.cfg"
);

void ReadWeaponOffsetConfig(real_vector3d *WeaponOffset)
{
    FILE* file = NULL;

    file = fopen(path, "r");
    if (file != NULL)
    {
        for (byte i = BattleRifle; i != Sniper + 1; i++)
        {
            fscanf(file, "%f,%f,%f\n", &WeaponOffset[i].i, &WeaponOffset[i].j, &WeaponOffset[i].k);
        }
        fclose(file);
    }
}

void SaveWeaponOffsetConfig(const real_vector3d WeaponOffset[])
{
    FILE* file = NULL;

    file = fopen(path, "w");
    if (file != NULL)
    {
        for (byte i = BattleRifle; i != Sniper + 1; i++)
        {
            fprintf(file, "%.3f,%.3f,%.3f\n", WeaponOffset[i].i, WeaponOffset[i].j, WeaponOffset[i].k);
        }
        fclose(file);
    }
}

// only writes anything if file dosent already exist
void WriteDefaultFile(const real_vector3d WeaponOffset[])
{
    FILE* file = NULL;    

    file = fopen(path, "r");
    if (file == NULL)
    {
        SaveWeaponOffsetConfig(WeaponOffset);
    }
    else fclose(file);
}