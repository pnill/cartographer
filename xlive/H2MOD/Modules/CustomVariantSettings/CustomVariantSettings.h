#pragma once

#include "Blam\Engine\Game\PhysicsConstants.h"
#include "Blam\Engine\Memory\bitstream.h"

namespace CustomVariantSettings
{
	enum e_hill_rotation
	{
		_random = 0,
		_sequential = 1,
		_reverse = 2,
		_predefined = 3,
		_invalid = -1
	};
	static const wchar_t* hill_rotation_name[] = {
		L"random",
		L"sequential",
		L"reverse",
		L"predefined"
	};
	struct s_variantSettings
	{
		double gravity = 1.0f;
		bool infiniteAmmo = false;
		bool explosionPhysics = false;
		e_hill_rotation hillRotation = _random;
		double gameSpeed = 1.0f;
		bool infiniteGrenades = false;
		bool spawnProtection = 1;
		byte predefinedHillSet[16];
		double forcedFOV = 0;

		double ComputedGravity() const
		{
			return gravity * s_physics_constants::get_default_gravity();
		}

		inline bool operator==(s_variantSettings& other) const
		{
			return memcmp(this, &other, sizeof(s_variantSettings)) == 0;
		}
		inline bool operator!=(s_variantSettings& other) const
		{
			return !(*this == other);
		}
	};
	void __cdecl EncodeVariantSettings(bitstream* stream, int a2, s_variantSettings* data);
	bool __cdecl DecodeVariantSettings(bitstream* stream, int a2, s_variantSettings* data);

	void ApplyCustomSettings(s_variantSettings* newVariantSettings);
	void UpdateCustomVariantSettings(s_variantSettings* data);
	void SendCustomVariantSettings(int peerIndex);
	void ApplyHooks();
	void Initialize();
}

extern CustomVariantSettings::s_variantSettings currentVariantSettings;
extern std::map<std::wstring, CustomVariantSettings::s_variantSettings> customVariantSettingsMap;

#define CustomVariantSettingsPacketSize (sizeof(CustomVariantSettings::s_variantSettings))