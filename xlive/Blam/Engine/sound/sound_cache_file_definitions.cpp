#include "stdafx.h"
#include "sound_cache_file_definitions.h"

#include "cseries/cseries_system_memory.h"
#include "game/game_globals.h"

runtime_global_sound_gestalt* get_runtime_global_sound_gestalt()
{
	return Memory::GetAddress<runtime_global_sound_gestalt*>(0x482298, 0);
}

void initialize_runtime_sound_gestalt_definition_shared(const s_sound_cache_file_gestalt_definition* shared_gestalt)
{
	runtime_global_sound_gestalt* global_sound_gestalt = get_runtime_global_sound_gestalt();

	global_sound_gestalt->playbacks_size = shared_gestalt->playbacks.count;
	if (global_sound_gestalt->playbacks_size)
		global_sound_gestalt->playbacks = shared_gestalt->playbacks.begin();
	else
		global_sound_gestalt->playbacks = nullptr;

	global_sound_gestalt->scales_size = shared_gestalt->scales.count;
	if (global_sound_gestalt->scales_size)
		global_sound_gestalt->scales = shared_gestalt->scales.begin();
	else
		global_sound_gestalt->scales = nullptr;

	global_sound_gestalt->import_names_size = shared_gestalt->import_names.count;
	if (global_sound_gestalt->import_names_size)
		global_sound_gestalt->import_names = shared_gestalt->import_names.begin();
	else
		global_sound_gestalt->import_names = nullptr;

	global_sound_gestalt->pitch_range_parameters_size = shared_gestalt->pitch_range_parameters.count;
	if (global_sound_gestalt->pitch_range_parameters_size)
		global_sound_gestalt->pitch_range_parameters = shared_gestalt->pitch_range_parameters.begin();
	else
		global_sound_gestalt->pitch_range_parameters = nullptr;

	global_sound_gestalt->pitch_ranges_size = shared_gestalt->pitch_ranges.count;
	if (global_sound_gestalt->pitch_ranges_size)
		global_sound_gestalt->pitch_ranges = shared_gestalt->pitch_ranges.begin();
	else
		global_sound_gestalt->pitch_ranges = nullptr;

	global_sound_gestalt->permutations_size = shared_gestalt->permutations.count;
	if (global_sound_gestalt->permutations_size)
		global_sound_gestalt->permutations = shared_gestalt->permutations.begin();
	else
		global_sound_gestalt->permutations = nullptr;

	global_sound_gestalt->custom_playbacks_size = shared_gestalt->custom_playbacks.count;
	if (global_sound_gestalt->custom_playbacks_size)
		global_sound_gestalt->custom_playbacks = shared_gestalt->custom_playbacks.begin();
	else
		global_sound_gestalt->custom_playbacks = nullptr;

	global_sound_gestalt->runtime_permutation_flags_size = shared_gestalt->runtime_permutation_flags.count;
	if (global_sound_gestalt->runtime_permutation_flags_size)
		global_sound_gestalt->runtime_permutation_flags = shared_gestalt->runtime_permutation_flags.begin();
	else
		global_sound_gestalt->runtime_permutation_flags = nullptr;

	global_sound_gestalt->chunks_size = shared_gestalt->chunks.count;
	if (global_sound_gestalt->chunks_size)
		global_sound_gestalt->chunks = shared_gestalt->chunks.begin();
	else
		global_sound_gestalt->chunks = nullptr;

	global_sound_gestalt->promotions_size = shared_gestalt->promotions.count;
	if (global_sound_gestalt->promotions_size)
		global_sound_gestalt->promotions = shared_gestalt->promotions.begin();
	else
		global_sound_gestalt->promotions = nullptr;

	global_sound_gestalt->extra_info_size = shared_gestalt->extra_info.count;
	if (global_sound_gestalt->extra_info_size)
		global_sound_gestalt->extra_info = shared_gestalt->extra_info.begin();
	else
		global_sound_gestalt->extra_info = nullptr;

	global_sound_gestalt->buffer = nullptr;
	global_sound_gestalt->initialized = true;
}

bool __cdecl initialize_runtime_sound_gestalt_definition_custom(s_sound_cache_file_gestalt_definition* shared_gestalt, s_sound_cache_file_gestalt_definition* custom_gestalt)
{
	return INVOKE(0x3CAE5, 0, initialize_runtime_sound_gestalt_definition_custom, shared_gestalt, custom_gestalt);
}
void __cdecl initialize_runtime_sound_gestalt_definition()
{
	//INVOKE(0x3DC37, 0, initialize_runtime_sound_gestalt_definition);

	runtime_global_sound_gestalt* global_sound_gestalt = get_runtime_global_sound_gestalt();
	s_game_globals* game_globals = scenario_get_game_globals();
	s_cache_header* cache_header = cache_files_get_header();

	if(global_sound_gestalt->initialized)
	{
		if(global_sound_gestalt->buffer)
			system_heap_free(global_sound_gestalt->buffer - *(int*)(global_sound_gestalt->buffer - 4));

		csmemset(&global_sound_gestalt, 0, sizeof(runtime_global_sound_gestalt));
	}

	if(game_globals->sound_globals.count)
	{
		datum shared_gestalt_datum = game_globals->sound_globals[0]->sound_gesalt;

		s_sound_cache_file_gestalt_definition* shared_gestalt = tags::get_tag_fast<s_sound_cache_file_gestalt_definition>(shared_gestalt_datum);
		s_sound_cache_file_gestalt_definition* custom_gestalt = nullptr;

		if(cache_header->secondary_ugh_tag_index != NONE)
			custom_gestalt = tags::get_tag_fast<s_sound_cache_file_gestalt_definition>(cache_header->secondary_ugh_tag_index);

		if(!custom_gestalt)
		{
			initialize_runtime_sound_gestalt_definition_shared(shared_gestalt);
		}
		else
		{
			if(!initialize_runtime_sound_gestalt_definition_custom(shared_gestalt, custom_gestalt))
			{
				auto a = 0;
				// Panic, wipe all snd! tags!!!
			}
		}
	}
	else
	{
		// Panic!!!
	}
}
