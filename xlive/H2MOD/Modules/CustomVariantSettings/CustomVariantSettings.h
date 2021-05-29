#pragma once
#include "H2MOD/Modules/Networking/Memory/bitstream.h"

#define CustomVariantSettingsPacketSize 37
namespace CustomVariantSettings
{
	enum e_hill_rotation
	{
		random = 0,
		sequential = 1,
		reverse = 2,
		invalid = -1
	};
	static const wchar_t* hill_rotation_name[] = {
		L"random",
		L"sequential",
		L"reverse",
	};
	struct s_variantSettings
	{
		double Gravity = 1.0f;
		bool InfiniteAmmo = false;
		bool ExplosionPhysics = false;
		e_hill_rotation HillRotation = random;
		double GameSpeed = 1.0f;
		bool InfiniteGrenades = false;
//		byte PredefinedHillSet[16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 ,-1 };
	};
	void __cdecl EncodeVariantSettings(bitstream* stream, int a2, s_variantSettings* data);
	bool __cdecl DecodeVariantSettings(bitstream* stream, int a2, s_variantSettings* data);

	void RecieveCustomVariantSettings(s_variantSettings* data);
	void SendCustomVariantSettings(int peerIndex);
	void ApplyHooks();
	void Initialize();
}
extern std::map<std::wstring, CustomVariantSettings::s_variantSettings> CustomVariantSettingsMap;
extern CustomVariantSettings::s_variantSettings CurrentVariantSettings;