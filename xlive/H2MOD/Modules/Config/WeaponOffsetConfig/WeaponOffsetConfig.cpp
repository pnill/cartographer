#include <iostream>
#include <fstream>
#include "WeaponOffsetConfig.h"
#include "stdafx.h"

// Default Values
float Magnum_WeaponOffsetX = 0.03;
float Magnum_WeaponOffsetY = 0;
float Magnum_WeaponOffsetZ = -0.005;
float SMG_WeaponOffsetX = 0.02;
float SMG_WeaponOffsetY = 0;
float SMG_WeaponOffsetZ = 0;
float Sniper_WeaponOffsetX = 0.01;
float Sniper_WeaponOffsetY = 0;
float Sniper_WeaponOffsetZ = 0;

void readOffset(std::ifstream &file, float &offset)
{
    file.ignore(32, '=');
    file >> offset;
}

void SaveWeaponOffsetConfig() {
    std::string path;
    std::ofstream file;

    path = getenv("localappdata");
    path += "\\Microsoft\\Halo 2\\WeaponOffsets.cfg";
    file.open( path.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (file.is_open())
    {
        file << "Magnum_WeaponOffsetX = "   << std::setprecision(3) << Magnum_WeaponOffsetX << std::endl;
        file << "Magnum_WeaponOffsetY = "   << std::setprecision(3) << Magnum_WeaponOffsetY << std::endl;
        file << "Magnum_WeaponOffsetZ = "   << std::setprecision(3) << Magnum_WeaponOffsetZ << std::endl;
        file << "SMG_WeaponOffsetX = "      << std::setprecision(3) << SMG_WeaponOffsetX << std::endl;
        file << "SMG_WeaponOffsetY = "      << std::setprecision(3) << SMG_WeaponOffsetY << std::endl;
        file << "SMG_WeaponOffsetZ = "      << std::setprecision(3) << SMG_WeaponOffsetZ << std::endl;
        file << "Sniper_WeaponOffsetX = "   << std::setprecision(3) << Sniper_WeaponOffsetX << std::endl;
        file << "Sniper_WeaponOffsetY = "   << std::setprecision(3) << Sniper_WeaponOffsetY << std::endl;
        file << "Sniper_WeaponOffsetZ = "   << std::setprecision(3) << Sniper_WeaponOffsetZ << std::endl;
    }
    file.close();

}

void ReadWeaponOffsetConfig() {
    std::string path;
    std::ifstream file;

    path = getenv("localappdata");
    path += "\\Microsoft\\Halo 2\\WeaponOffsets.cfg";
    file.open( path.c_str() );
    if (file.is_open())
    {
        readOffset(file, Magnum_WeaponOffsetX);
        readOffset(file, Magnum_WeaponOffsetY);
        readOffset(file, Magnum_WeaponOffsetZ);

        readOffset(file, SMG_WeaponOffsetX);
        readOffset(file, SMG_WeaponOffsetY);
        readOffset(file, SMG_WeaponOffsetZ);

        readOffset(file, Sniper_WeaponOffsetX);
        readOffset(file, Sniper_WeaponOffsetY);
        readOffset(file, Sniper_WeaponOffsetZ);
    }
    file.close();
}