#pragma once
#include "H2MOD/Modules/Networking/Memory/bitstream.h"

#define CustomVariantSettingsPacketSize 20
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
		double Gravity = 4.171259403f;
		bool InfiniteAmmo = false;
		bool ExplosionPhysics = false;
		e_hill_rotation HillRotation = random;
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