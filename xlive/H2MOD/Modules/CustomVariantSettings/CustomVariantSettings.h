#pragma once
#include "physics/physics_constants.h"
#include "memory/bitstream.h"

namespace CustomVariantSettings
{
	enum e_hill_rotation : int32
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
	struct s_variant_settings
	{
		real32 gravity = 1.0f;
		bool infiniteAmmo = false;
		bool explosionPhysics = false;
		e_hill_rotation hillRotation = _random;
		real32 gameSpeed = 1.0f;
		bool infiniteGrenades = false;
		bool spawnProtection = true;
		uint8 predefinedHillSet[16] = {};
		uint8 forced_fov = 0;
		bool disable_dub_shot = false;

		real32 ComputedGravity() const
		{
			return gravity * s_physics_constants::get_default_gravity();
		}

		inline bool operator==(s_variant_settings& other) const
		{
			return memcmp(this, &other, sizeof(s_variant_settings)) == 0;
		}
		inline bool operator!=(s_variant_settings& other) const
		{
			return !(*this == other);
		}
	};
	void EncodeVariantSettings(c_bitstream* stream, int a2, s_variant_settings* data);
	void DecodeVariantSettings(c_bitstream* stream, int a2, s_variant_settings* data);

	void ApplyCustomSettings(s_variant_settings* newVariantSettings);
	void UpdateCustomVariantSettings(const s_variant_settings* data);
	void SendCustomVariantSettings(int32 peer_index);
	void ApplyHooks();
	void Initialize();
}

extern CustomVariantSettings::s_variant_settings currentVariantSettings;

#define k_custom_variant_settings_packet_size (sizeof(CustomVariantSettings::s_variant_settings))