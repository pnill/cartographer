#pragma once
#include "memory/static_arrays.h"

enum e_sound_definition_compression : uint8
{
	_sound_definition_compression_none_big_endian = 0,
	_sound_definition_compression_xbox_adpcm = 1,
	_sound_definition_compression_ima_adpcm = 2,
	_sound_definition_compression_none_little_endian = 3,
	_sound_definition_compression_wma = 4
};

enum e_sound_definition_encoding : uint8
{
	_sound_definition_encoding_mono = 0,
	_sound_definition_encoding_stereo = 1,
	_sound_definition_encoding_codec = 2
};

enum e_sound_definition_sample_rate : uint8
{
	_sound_definition_sample_rate_22khz = 0,
	_sound_definition_sample_rate_44khz = 1,
	_sound_definition_sample_rate_32khz = 2
};

enum e_sound_definition_class : uint8
{
	_sound_definition_class_projectile_impact = 0,
	_sound_definition_class_projectile_detonation = 1,
	_sound_definition_class_projectile_flyby = 2,
	_sound_definition_class_weapon_fire = 4,
	_sound_definition_class_weapon_ready = 5,
	_sound_definition_class_weapon_reload = 6,
	_sound_definition_class_weapon_empty = 7,
	_sound_definition_class_weapon_charge = 8,
	_sound_definition_class_weapon_overheat = 9,
	_sound_definition_class_weapon_idle = 10,
	_sound_definition_class_weapon_melee = 11,
	_sound_definition_class_weapon_animation = 12,
	_sound_definition_class_object_impacts = 13,
	_sound_definition_class_particle_impacts = 14,
	_sound_definition_class_unit_footsteps = 18,
	_sound_definition_class_unit_dialog = 19,
	_sound_definition_class_unit_animation = 20,
	_sound_definition_class_vehicle_collision = 22,
	_sound_definition_class_vehicle_engine = 23,
	_sound_definition_class_vehicle_animation = 24,
	_sound_definition_class_device_door = 26,
	_sound_definition_class_device_machinery = 28,
	_sound_definition_class_device_stationary = 29,
	_sound_definition_class_music = 32,
	_sound_definition_class_ambient_nature = 33,
	_sound_definition_class_ambient_machinery = 34,
	_sound_definition_class_huge_ass = 36,
	_sound_definition_class_object_looping = 37,
	_sound_definition_class_cinematic_music = 38,
	_sound_definition_class_cortana_mission = 45,
	_sound_definition_class_cortana_cinematic = 46,
	_sound_definition_class_mission_dialog = 47,
	_sound_definition_class_cinematic_dialog = 48,
	_sound_definition_class_scripted_cinematic_foley = 49,
	_sound_definition_class_game_event = 50,
	_sound_definition_class_ui = 51,
	_sound_definition_class_test = 52,
	_sound_definition_class_multilingual_test = 53,
};

enum e_sound_definition_flags : uint16
{
	_sound_definition_flag_fit_to_adpcm_block_size = 0,
	_sound_definition_flag_split_long_sound_into_permutations = 1,
	_sound_definition_flag_always_spatialize_always_play_as_3d_sound_even_in_first_person = 2,
	_sound_definition_flag_never_obstruct_disable_occlusion_obstruction = 3,
	_sound_definition_flag_internal_dont_touch = 4,
	_sound_definition_flag_use_huge_sound_transmission = 5,
	_sound_definition_flag_link_count_to_owner_unit = 6,
	_sound_definition_flag_pitch_range_is_language = 7,
	_sound_definition_flag_dont_use_sound_class_speaker_flag = 8,
	_sound_definition_flag_dont_use_lipsync_data = 9,
	k_sound_definition_flag_count
};
struct sound_definition_v1
{
	c_flags_no_init<e_sound_definition_flags, uint16, k_sound_definition_flag_count> flags;

	e_sound_definition_class sound_class;
	e_sound_definition_sample_rate sample_rate;
	e_sound_definition_encoding encoding;
	e_sound_definition_compression compression;

	uint16 gestalt_playback_parameter_index;
	uint16 gestalt_pitch_range_index;
	uint8 gestalt_unknown_index;
	uint8 gestalt_scale_index;
	uint8 gestalt_promotion_index;
	uint8 gestalt_custom_playback_index;
	uint16 gestalt_extra_info_index;
	uint32 maximum_play_time;
};

ASSERT_STRUCT_SIZE(sound_definition_v1, 20);
