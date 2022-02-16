#include <iostream>
#include <fstream>
#include "WeaponOffsetConfig.h"
#include "stdafx.h"

// Default Values
float BattleRifle_WeaponOffsetX = 0.02;
float BattleRifle_WeaponOffsetY = 0;
float BattleRifle_WeaponOffsetZ = -0.004;
float BeamRifle_WeaponOffsetX = 0.02;
float BeamRifle_WeaponOffsetY = 0;
float BeamRifle_WeaponOffsetZ = 0;
float BrutePlasmaRifle_WeaponOffsetX = 0.02;
float BrutePlasmaRifle_WeaponOffsetY = 0;
float BrutePlasmaRifle_WeaponOffsetZ = 0;
float BruteShot_WeaponOffsetX = 0.02;
float BruteShot_WeaponOffsetY = -0.01;
float BruteShot_WeaponOffsetZ = 0.001;
float Carbine_WeaponOffsetX = 0.02;
float Carbine_WeaponOffsetY = 0;
float Carbine_WeaponOffsetZ = -0.004;
float EnergySword_WeaponOffsetX = 0.05;
float EnergySword_WeaponOffsetY = 0;
float EnergySword_WeaponOffsetZ = 0;
float FuelRod_WeaponOffsetX = 0.02;
float FuelRod_WeaponOffsetY = 0;
float FuelRod_WeaponOffsetZ = -0.005;
float Magnum_WeaponOffsetX = 0.03;
float Magnum_WeaponOffsetY = 0;
float Magnum_WeaponOffsetZ = -0.005;
float Needler_WeaponOffsetX = 0.03;
float Needler_WeaponOffsetY = 0;
float Needler_WeaponOffsetZ = -0.005;
float PlasmaPistol_WeaponOffsetX = 0.03;
float PlasmaPistol_WeaponOffsetY = 0;
float PlasmaPistol_WeaponOffsetZ = -0.005;
float PlasmaRifle_WeaponOffsetX = 0.02;
float PlasmaRifle_WeaponOffsetY = 0;
float PlasmaRifle_WeaponOffsetZ = 0;
float RocketLauncher_WeaponOffsetX = 0.02;
float RocketLauncher_WeaponOffsetY = 0;
float RocketLauncher_WeaponOffsetZ = -0.005;
float SentinelBeam_WeaponOffsetX = 0.02;
float SentinelBeam_WeaponOffsetY = 0.03;
float SentinelBeam_WeaponOffsetZ = 0;
float Shotgun_WeaponOffsetX = 0.02;
float Shotgun_WeaponOffsetY = -0.001;
float Shotgun_WeaponOffsetZ = -0.015;
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
        file << "BattleRifle_WeaponOffsetX = "      << std::setprecision(2) << BattleRifle_WeaponOffsetX << std::endl;
        file << "BattleRifle_WeaponOffsetY = "      << std::setprecision(2) << BattleRifle_WeaponOffsetY << std::endl;
        file << "BattleRifle_WeaponOffsetZ = "      << std::setprecision(2) << BattleRifle_WeaponOffsetZ << std::endl;
        file << "BeamRifle_WeaponOffsetX = "        << std::setprecision(2) << BeamRifle_WeaponOffsetX << std::endl;
        file << "BeamRifle_WeaponOffsetY = "        << std::setprecision(2) << BeamRifle_WeaponOffsetY << std::endl;
        file << "BeamRifle_WeaponOffsetZ = "        << std::setprecision(2) << BeamRifle_WeaponOffsetZ << std::endl;
        file << "BrutePlasmaRifle_WeaponOffsetX = " << std::setprecision(2) << BrutePlasmaRifle_WeaponOffsetX << std::endl;
        file << "BrutePlasmaRifle_WeaponOffsetY = " << std::setprecision(2) << BrutePlasmaRifle_WeaponOffsetY << std::endl;
        file << "BrutePlasmaRifle_WeaponOffsetZ = " << std::setprecision(2) << BrutePlasmaRifle_WeaponOffsetZ << std::endl;
        file << "BruteShot_WeaponOffsetX = "        << std::setprecision(2) << BruteShot_WeaponOffsetX << std::endl;
        file << "BruteShot_WeaponOffsetY = "        << std::setprecision(2) << BruteShot_WeaponOffsetY << std::endl;
        file << "BruteShot_WeaponOffsetZ = "        << std::setprecision(2) << BruteShot_WeaponOffsetZ << std::endl;
        file << "Carbine_WeaponOffsetX = "          << std::setprecision(2) << Carbine_WeaponOffsetX << std::endl;
        file << "Carbine_WeaponOffsetY = "          << std::setprecision(2) << Carbine_WeaponOffsetY << std::endl;
        file << "Carbine_WeaponOffsetZ = "          << std::setprecision(2) << Carbine_WeaponOffsetZ << std::endl;
        file << "EnergySword_WeaponOffsetX = "      << std::setprecision(2) << EnergySword_WeaponOffsetX << std::endl;
        file << "EnergySword_WeaponOffsetY = "      << std::setprecision(2) << EnergySword_WeaponOffsetY << std::endl;
        file << "EnergySword_WeaponOffsetZ = "      << std::setprecision(2) << EnergySword_WeaponOffsetZ << std::endl;
        file << "FuelRod_WeaponOffsetX = "          << std::setprecision(2) << FuelRod_WeaponOffsetX << std::endl;
        file << "FuelRod_WeaponOffsetY = "          << std::setprecision(2) << FuelRod_WeaponOffsetY << std::endl;
        file << "FuelRod_WeaponOffsetZ = "          << std::setprecision(2) << FuelRod_WeaponOffsetZ << std::endl;
        file << "Magnum_WeaponOffsetX = "           << std::setprecision(2) << Magnum_WeaponOffsetX << std::endl;
        file << "Magnum_WeaponOffsetY = "           << std::setprecision(2) << Magnum_WeaponOffsetY << std::endl;
        file << "Magnum_WeaponOffsetZ = "           << std::setprecision(2) << Magnum_WeaponOffsetZ << std::endl;
        file << "Needler_WeaponOffsetX = "          << std::setprecision(2) << Needler_WeaponOffsetX << std::endl;
        file << "Needler_WeaponOffsetY = "          << std::setprecision(2) << Needler_WeaponOffsetY << std::endl;
        file << "Needler_WeaponOffsetZ = "          << std::setprecision(2) << Needler_WeaponOffsetZ << std::endl;
        file << "PlasmaPistol_WeaponOffsetX = "     << std::setprecision(2) << PlasmaPistol_WeaponOffsetX << std::endl;
        file << "PlasmaPistol_WeaponOffsetY = "     << std::setprecision(2) << PlasmaPistol_WeaponOffsetY << std::endl;
        file << "PlasmaPistol_WeaponOffsetZ = "     << std::setprecision(2) << PlasmaPistol_WeaponOffsetZ << std::endl;
        file << "PlasmaRifle_WeaponOffsetX = "      << std::setprecision(2) << PlasmaRifle_WeaponOffsetX << std::endl;
        file << "PlasmaRifle_WeaponOffsetY = "      << std::setprecision(2) << PlasmaRifle_WeaponOffsetY << std::endl;
        file << "PlasmaRifle_WeaponOffsetZ = "      << std::setprecision(2) << PlasmaRifle_WeaponOffsetZ << std::endl;
        file << "RocketLauncher_WeaponOffsetX = "   << std::setprecision(2) << RocketLauncher_WeaponOffsetX << std::endl;
        file << "RocketLauncher_WeaponOffsetY = "   << std::setprecision(2) << RocketLauncher_WeaponOffsetY << std::endl;
        file << "RocketLauncher_WeaponOffsetZ = "   << std::setprecision(2) << RocketLauncher_WeaponOffsetZ << std::endl;
        file << "SentinelBeam_WeaponOffsetX = "     << std::setprecision(2) << SentinelBeam_WeaponOffsetX << std::endl;
        file << "SentinelBeam_WeaponOffsetY = "     << std::setprecision(2) << SentinelBeam_WeaponOffsetY << std::endl;
        file << "SentinelBeam_WeaponOffsetZ = "     << std::setprecision(2) << SentinelBeam_WeaponOffsetZ << std::endl;
        file << "Shotgun_WeaponOffsetX = "          << std::setprecision(2) << Shotgun_WeaponOffsetX << std::endl;
        file << "Shotgun_WeaponOffsetY = "          << std::setprecision(2) << Shotgun_WeaponOffsetY << std::endl;
        file << "Shotgun_WeaponOffsetZ = "          << std::setprecision(2) << Shotgun_WeaponOffsetZ << std::endl;
        file << "SMG_WeaponOffsetX = "              << std::setprecision(2) << SMG_WeaponOffsetX << std::endl;
        file << "SMG_WeaponOffsetY = "              << std::setprecision(2) << SMG_WeaponOffsetY << std::endl;
        file << "SMG_WeaponOffsetZ = "              << std::setprecision(2) << SMG_WeaponOffsetZ << std::endl;
        file << "Sniper_WeaponOffsetX = "           << std::setprecision(2) << Sniper_WeaponOffsetX << std::endl;
        file << "Sniper_WeaponOffsetY = "           << std::setprecision(2) << Sniper_WeaponOffsetY << std::endl;
        file << "Sniper_WeaponOffsetZ = "           << std::setprecision(2) << Sniper_WeaponOffsetZ << std::endl;
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
        readOffset(file, BattleRifle_WeaponOffsetX);
        readOffset(file, BattleRifle_WeaponOffsetY);
        readOffset(file, BattleRifle_WeaponOffsetZ);
        readOffset(file, BeamRifle_WeaponOffsetX);
        readOffset(file, BeamRifle_WeaponOffsetY);
        readOffset(file, BeamRifle_WeaponOffsetZ);
        readOffset(file, BrutePlasmaRifle_WeaponOffsetX);
        readOffset(file, BrutePlasmaRifle_WeaponOffsetY);
        readOffset(file, BrutePlasmaRifle_WeaponOffsetZ);
        readOffset(file, BruteShot_WeaponOffsetX);
        readOffset(file, BruteShot_WeaponOffsetY);
        readOffset(file, BruteShot_WeaponOffsetZ);
        readOffset(file, BruteShot_WeaponOffsetX);
        readOffset(file, BruteShot_WeaponOffsetY);
        readOffset(file, BruteShot_WeaponOffsetZ);
        readOffset(file, Carbine_WeaponOffsetX);
        readOffset(file, Carbine_WeaponOffsetY);
        readOffset(file, Carbine_WeaponOffsetZ);
        readOffset(file, EnergySword_WeaponOffsetX);
        readOffset(file, EnergySword_WeaponOffsetY);
        readOffset(file, EnergySword_WeaponOffsetZ);
        readOffset(file, FuelRod_WeaponOffsetX);
        readOffset(file, FuelRod_WeaponOffsetY);
        readOffset(file, FuelRod_WeaponOffsetZ);
        readOffset(file, Magnum_WeaponOffsetX);
        readOffset(file, Magnum_WeaponOffsetY);
        readOffset(file, Magnum_WeaponOffsetZ);
        readOffset(file, Needler_WeaponOffsetX);
        readOffset(file, Needler_WeaponOffsetY);
        readOffset(file, Needler_WeaponOffsetZ);
        readOffset(file, PlasmaPistol_WeaponOffsetX);
        readOffset(file, PlasmaPistol_WeaponOffsetY);
        readOffset(file, PlasmaPistol_WeaponOffsetZ);
        readOffset(file, PlasmaRifle_WeaponOffsetX);
        readOffset(file, PlasmaRifle_WeaponOffsetY);
        readOffset(file, PlasmaRifle_WeaponOffsetZ);
        readOffset(file, RocketLauncher_WeaponOffsetX);
        readOffset(file, RocketLauncher_WeaponOffsetY);
        readOffset(file, RocketLauncher_WeaponOffsetZ);
        readOffset(file, SentinelBeam_WeaponOffsetX);
        readOffset(file, SentinelBeam_WeaponOffsetY);
        readOffset(file, SentinelBeam_WeaponOffsetZ);
        readOffset(file, Shotgun_WeaponOffsetX);
        readOffset(file, Shotgun_WeaponOffsetY);
        readOffset(file, Shotgun_WeaponOffsetZ);
        readOffset(file, SMG_WeaponOffsetX);
        readOffset(file, SMG_WeaponOffsetY);
        readOffset(file, SMG_WeaponOffsetZ);
        readOffset(file, Sniper_WeaponOffsetX);
        readOffset(file, Sniper_WeaponOffsetY);
        readOffset(file, Sniper_WeaponOffsetZ);
    }
    file.close();
}