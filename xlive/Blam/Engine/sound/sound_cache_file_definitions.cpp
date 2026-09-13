#include "stdafx.h"
#include "sound_cache_file_definitions.h"

#include "platform_sound_effect_definitions.h"
#include "sound_definitions.h"

#include "cache/cache_files.h"
#include "cseries/cseries_system_memory.h"
#include "game/game_globals.h"

/* macros */

#define sound_cache_file_gestalt_definition_get(index)	((struct s_sound_cache_file_gestalt_definition*)tag_get(SOUND_CACHE_FILE_GESTALT_TAG, (index)))

/* structures */

struct s_sound_cache_file_gestalt_definition
{
	s_tag_block playbacks;					// s_sound_definition_playback_parameters
	s_tag_block scales;						// s_sound_definition_scale_modifiers

	// max: SHORT_MAX
	s_tag_block import_names;				// string_id
	s_tag_block pitch_range_parameters;		// s_cache_file_sound_pitch_range_parameters
	s_tag_block pitch_ranges;				// s_cache_file_sound_pitch_range
	s_tag_block permutations;				// s_cache_file_sound_permutation
	s_tag_block custom_playbacks;			// s_platform_sound_playback_definition

	// max: SHORT_MAX
	s_tag_block runtime_permutation_flags;	// int8
	s_tag_block chunks;						// s_sound_permutation_chunk
	s_tag_block promotions;					// s_sound_promotion_parameters
	s_tag_block extra_info;					// s_cache_file_sound_definition_extra_info
};
ASSERT_STRUCT_SIZE(s_sound_cache_file_gestalt_definition, 88);

struct runtime_global_sound_gestalt
{
	bool initialized;
	struct s_sound_definition_playback_parameters* playbacks;
	uint32 playbacks_size;
	struct s_sound_definition_scale_modifiers* scales;
	uint32 scales_size;
	string_id* import_names;
	uint32 import_names_size;
	struct s_cache_file_sound_pitch_range_parameters* pitch_range_parameters;
	uint32 pitch_range_parameters_size;
	struct s_cache_file_sound_pitch_range* pitch_ranges;
	uint32 pitch_ranges_size;
	struct s_cache_file_sound_permutation* permutations;
	uint32 permutations_size;
	struct s_platform_sound_playback_definition* custom_playbacks;
	uint32 custom_playbacks_size;
	int8* runtime_permutation_flags;
	uint32 runtime_permutation_flags_size;
	struct s_sound_permutation_chunk* chunks;
	uint32 chunks_size;
	struct s_sound_promotion_parameters* promotions;
	uint32 promotions_size;
	struct s_cache_file_sound_definition_extra_info* extra_info;
	uint32 extra_info_size;
	int8* buffer;
};
ASSERT_STRUCT_SIZE(runtime_global_sound_gestalt, 96);

/* prototypes */

static runtime_global_sound_gestalt* get_runtime_global_sound_gestalt(void);

static void initialize_runtime_sound_gestalt_panic(void);

static void initialize_runtime_sound_gestalt_definition_primary_only(s_sound_cache_file_gestalt_definition const* gestalt);

/* public code */

bool __cdecl initialize_runtime_sound_gestalt_definition_with_secondary(s_sound_cache_file_gestalt_definition* primary_gestalt, s_sound_cache_file_gestalt_definition* secondary_gestalt)
{
	return INVOKE(0x3CAE5, 0x4F0DA, initialize_runtime_sound_gestalt_definition_with_secondary, primary_gestalt, secondary_gestalt);
}

void initialize_runtime_sound_gestalt_definition(void)
{
	runtime_global_sound_gestalt* global_sound_gestalt = get_runtime_global_sound_gestalt();
	cache_file_header const* cache_header = cache_files_get_header();

	if (global_sound_gestalt->initialized)
	{
		if (global_sound_gestalt->buffer)
		{
			CSERIES_FREE(global_sound_gestalt->buffer - *(int*)(global_sound_gestalt->buffer - 4));
		}

		csmemset(global_sound_gestalt, 0, sizeof(runtime_global_sound_gestalt));
	}

	struct s_sound_globals_definition const* sound_globals = game_globals_get_sound_globals(scenario_get_game_globals(), offsetof(s_game_globals, s_game_globals::sound_globals), 0);

	ASSERT(sound_globals);
	ASSERT(sound_globals->runtime_gestalt_sound_definition_index != NONE);

	s_sound_cache_file_gestalt_definition* primary_gestalt = sound_cache_file_gestalt_definition_get(sound_globals->runtime_gestalt_sound_definition_index);

	// if the cache file uses a shared resource database the primary gestalt will be the datum of that shared database and the cache gestalt will be the secondary
	// if the cache file does not use a shared resource database the cache gestalt will be the primary.

	if (cache_header->secondary_ugh_tag_index != NONE)
	{
		s_sound_cache_file_gestalt_definition* secondary_gestalt = sound_cache_file_gestalt_definition_get(cache_header->secondary_ugh_tag_index);

		if (!initialize_runtime_sound_gestalt_definition_with_secondary(primary_gestalt, secondary_gestalt))
		{
			// Gestalt initialization failed, clear all gestalt info for sounds.
			initialize_runtime_sound_gestalt_panic();
		}
	}
	else
	{
		initialize_runtime_sound_gestalt_definition_primary_only(primary_gestalt);
	}
	
	return;
}

/* private code */

static runtime_global_sound_gestalt* get_runtime_global_sound_gestalt(void)
{
	return Memory::GetAddress<runtime_global_sound_gestalt*>(0x482298, 0x4D2518);
}

static void initialize_runtime_sound_gestalt_panic(void)
{
	// Clear all sound tags gestalt parameters so they cannot play.

	tag_iterator sound_iterator;
	tag_iterator_new(&sound_iterator, _tag_group_sound);

	while (tag_iterator_next(&sound_iterator) != NONE)
	{
		sound_definition_v1* sound = sound_definition_get(sound_iterator.current_tag_index);

		sound->gestalt_custom_playback_index = NONE;
		sound->gestalt_extra_info_index = NONE;
		sound->gestalt_pitch_range_index = NONE;
		sound->gestalt_playback_parameter_index = NONE;
		sound->gestalt_promotion_index = NONE;
		sound->gestalt_scale_index = NONE;
		sound->gestalt_unknown_index = NONE;
	}

	return;
}

static void initialize_runtime_sound_gestalt_definition_primary_only(
	s_sound_cache_file_gestalt_definition const* gestalt)
{
	runtime_global_sound_gestalt* g_sound_gestalt = get_runtime_global_sound_gestalt();

	ASSERT(gestalt);
	ASSERT(!g_sound_gestalt->initialized);
	ASSERT(g_sound_gestalt->buffer == NULL);

	g_sound_gestalt->playbacks_size = gestalt->playbacks.count;
	g_sound_gestalt->playbacks = g_sound_gestalt->playbacks_size ? TAG_BLOCK_GET_ELEMENT(&gestalt->playbacks, 0, s_sound_definition_playback_parameters) : NULL;
	
	g_sound_gestalt->scales_size = gestalt->scales.count;
	g_sound_gestalt->scales = g_sound_gestalt->scales_size ? TAG_BLOCK_GET_ELEMENT(&gestalt->scales, 0, s_sound_definition_scale_modifiers) : NULL;

	g_sound_gestalt->import_names_size = gestalt->import_names.count;
	g_sound_gestalt->import_names = g_sound_gestalt->import_names_size ? TAG_BLOCK_GET_ELEMENT(&gestalt->import_names, 0, string_id) : NULL;
	
	g_sound_gestalt->pitch_range_parameters_size = gestalt->pitch_range_parameters.count;
	g_sound_gestalt->pitch_range_parameters = g_sound_gestalt->pitch_range_parameters_size ? TAG_BLOCK_GET_ELEMENT(&gestalt->pitch_range_parameters, 0, s_cache_file_sound_pitch_range_parameters) : NULL;
	
	g_sound_gestalt->pitch_ranges_size = gestalt->pitch_ranges.count;
	g_sound_gestalt->pitch_ranges = g_sound_gestalt->pitch_ranges_size ? TAG_BLOCK_GET_ELEMENT(&gestalt->pitch_ranges, 0, s_cache_file_sound_pitch_range) : NULL;
	
	g_sound_gestalt->permutations_size = gestalt->permutations.count;
	g_sound_gestalt->permutations = g_sound_gestalt->permutations_size ? TAG_BLOCK_GET_ELEMENT(&gestalt->permutations, 0, s_cache_file_sound_permutation) : NULL;
	
	g_sound_gestalt->custom_playbacks_size = gestalt->custom_playbacks.count;
	g_sound_gestalt->custom_playbacks = g_sound_gestalt->custom_playbacks_size ? TAG_BLOCK_GET_ELEMENT(&gestalt->custom_playbacks, 0, s_platform_sound_playback_definition) : NULL;
	
	g_sound_gestalt->runtime_permutation_flags_size = gestalt->runtime_permutation_flags.count;
	g_sound_gestalt->runtime_permutation_flags = g_sound_gestalt->runtime_permutation_flags_size ? TAG_BLOCK_GET_ELEMENT(&gestalt->runtime_permutation_flags, 0, int8) : NULL;

	g_sound_gestalt->chunks_size = gestalt->chunks.count;
	g_sound_gestalt->chunks = g_sound_gestalt->chunks_size ? TAG_BLOCK_GET_ELEMENT(&gestalt->chunks, 0, s_sound_permutation_chunk) : NULL;
	
	g_sound_gestalt->promotions_size = gestalt->promotions.count;
	g_sound_gestalt->promotions = g_sound_gestalt->promotions_size ? TAG_BLOCK_GET_ELEMENT(&gestalt->promotions, 0, s_sound_promotion_parameters) : NULL;
	
	g_sound_gestalt->extra_info_size = gestalt->extra_info.count;
	g_sound_gestalt->extra_info = g_sound_gestalt->extra_info_size ? TAG_BLOCK_GET_ELEMENT(&gestalt->extra_info, 0, s_cache_file_sound_definition_extra_info) : NULL;

	g_sound_gestalt->buffer = NULL;
	g_sound_gestalt->initialized = true;
	
	return;
}