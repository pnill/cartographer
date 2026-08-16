#include "stdafx.h"
#include "havok.h"

#include "havok_component.h"

#include "cache/cache_files.h"
#include "objects/object_definition.h"
#include "objects/object_types.h"

/* prototypes */

static s_havok_globals* havok_globals_get(void);

/* public code */

data_array* havok_components_get(void)
{
	return *Memory::GetAddress<data_array**>(0x4D8624, 0x4FFF84);
}

s_havok_game_state* havok_game_state_get(void)
{
	return *Memory::GetAddress<s_havok_game_state**>(0x4CE83C, 0x4F5028);
}

bool havok_can_modify_state(void)
{
	return havok_memory_allocator_locked() || is_havok_update_memory_initialized();
}

bool havok_can_allocate_space_for_instance_of_object_definition(datum tag_index)
{
	//return INVOKE(0x9FE55, 0x920B5, havok_can_allocate_space_for_instance_of_object_definition, tag_index);
	
	const object_definition* definition = object_definition_get(tag_index);

	ASSERT(definition);
	ASSERT(havok_can_modify_state());

	bool result = true;
	if (TEST_BIT(_object_mask_havok, definition->object.object_type))
	{
		const s_havok_game_state* g_havok_game_state = havok_game_state_get();
		if (havok_globals_get()->rigid_bodies_active)
		{
			const data_array* g_havok_component_data = havok_components_get();

			ASSERT(g_havok_component_data->maximum_count == k_maximum_havok_component_count);
			ASSERT(g_havok_game_state->havok_components_allocated == g_havok_component_data->actual_count);

			result = g_havok_component_data->actual_count < g_havok_component_data->maximum_count;
		}
		else
		{
			result = g_havok_game_state->havok_components_allocated < k_maximum_havok_component_count;
		}
	}

	return result;
}

void __cdecl havok_update(void)
{
	INVOKE(0xA27EB, 0x94A4B, havok_update);
	return;
}

/* private code */

static s_havok_globals* havok_globals_get(void)
{
	return Memory::GetAddress<s_havok_globals*>(0x418AC8, 0x3BBDEC);
}
