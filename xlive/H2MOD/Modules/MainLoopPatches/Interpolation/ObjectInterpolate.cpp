#include "stdafx.h"

#include "ObjectInterpolate.h"

#include "Util\Hooks\Hook.h"
#include "Interpolation.h"

#include "Blam\Math\BlamMath.h"
#include "Blam\Engine\Objects\Objects.h"
#include "Blam\Engine\DataArray\DataArray.h"
#include "Blam\Engine\Objects\ObjectPlacementData.h"
#include "Blam\Engine\Game\GameTimeGlobals.h"

#include "H2MOD/GUI/imgui_integration/Console/ImGui_ConsoleImpl.h"

#define OBJECT_MAX_NODES (255)
#define OBJECT_GAME_STATE_MAX_COUNT (2048)

namespace ObjectInterpolate
{
	namespace
	{
		// static namespace members
		struct s_object_interpolation
		{
			bool valid;
			datum object_idx = DATUM_INDEX_NONE;
			int node_count;
			real_matrix4x3* node_ptr;
			real_matrix4x3 interpolated_nodes[OBJECT_MAX_NODES];
			real_matrix4x3 previous_node_position[OBJECT_MAX_NODES];
			// TODO add unique id's to identify objects
		} object_states[OBJECT_GAME_STATE_MAX_COUNT];

		bool SameObject(datum object_idx)
		{
			int object_node_count;
			s_object_interpolation* object_state = &object_states[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_idx)];
			real_matrix4x3* object_nodes = get_object_nodes(object_idx, &object_node_count);
			bool ret = object_state->valid 
				&& object_state->object_idx == object_idx 
				&& object_state->node_count == object_node_count
				&& object_state->node_ptr == object_nodes;

			return ret;
		}

		bool initialized = false;
		int object_count;
		datum object_last_datum_idx;
	}

	void PreGameTickLocalPlayersUpdate()
	{
		if (!Interpolation::ShouldInterpolate())
			return;

		s_data_iterator<s_object_header*> object_it(get_objects_header());

		// check if we actually executed any ticks
		if (!(time_globals::get_game_time() > 0))
		{
			Reset();
			return;
		}
		
		while (object_it.get_next_datum())
		{
			s_object_interpolation* object_state = &object_states[object_it.get_current_absolute_index()];
			s_object_data_definition* object_data = object_get_fast_unsafe(object_it.get_current_datum_index());

			if (!SameObject(object_it.get_current_datum_index()))
			{
				ResetObject(object_it.get_current_datum_index());
			}

			if (!object_state->valid)
			{
				object_count++;
				object_state->valid = true;
			}

			int object_node_count;
			real_matrix4x3* object_nodes = get_object_nodes(object_it.get_current_datum_index(), &object_node_count);
			/*if (object_node_count > OBJECT_MAX_NODES)
				DBGBREAK();*/

			memcpy(object_state->previous_node_position, object_nodes, sizeof(real_matrix4x3) * object_node_count);
			object_state->object_idx = object_it.get_current_datum_index();
			object_state->node_count = object_node_count;
			object_state->node_ptr = object_nodes;
		}
	}

	void PostGameTickLocalPlayersUpdate()
	{
		if (!Interpolation::ShouldInterpolate())
		{
			return;
		}

		s_data_iterator<s_object_header*> object_it(get_objects_header());

		while (object_it.get_next_datum())
		{
			s_object_interpolation* object_state = &object_states[object_it.get_current_absolute_index()];
			s_object_data_definition* object_data = object_get_fast_unsafe(object_it.get_current_datum_index());

			// TODO FIXME add unique object ID to SameObject validation
			if (!SameObject(object_it.get_current_datum_index()))
			{
				ResetObject(object_it.get_current_datum_index());
				continue;
			}
		}
	}

	void ResetObject(datum object_idx)
	{
		s_object_interpolation* object_state = &object_states[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_idx)];
		if (!object_state->valid)
			return;
		object_state->object_idx = DATUM_INDEX_NONE;
		memset(object_state->previous_node_position, 0, sizeof(real_matrix4x3) * object_state->node_count);
		memset(object_state->interpolated_nodes, 0, sizeof(real_matrix4x3) * object_state->node_count);
		object_state->node_count = 0;
		object_state->node_ptr = NULL;
		object_state->valid = false;
		object_count--;
	}

	const real_matrix4x3* __cdecl object_get_node_matrices_hook(datum object_idx, int* out_node_count)
	{
		s_object_interpolation* object_state = &object_states[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_idx)];

		if (!Interpolation::ShouldInterpolate())
		{
			return get_object_nodes(object_idx, out_node_count);
		}
		
		if (!SameObject(object_idx))
		{
			real_matrix4x3* ret = get_object_nodes(object_idx, out_node_count);
			CircularStringBuffer* output = GetMainConsoleInstance()->GetTabOutput(_console_tab_logs);
			output->AddStringFmt(StringFlag_None, "object interpolate: %x mismatch, cached object idx: %x, state valid: %d, node count: %d, cached: %d, node ptr: %#010x, cached: %#010x", 
				object_idx, object_state->object_idx, object_state->valid, *out_node_count, object_state->node_count, object_state->node_ptr, ret);
			return ret;
		}
		
		int node_count;
		real_matrix4x3* current_object_nodes = get_object_nodes(object_idx, &node_count);

		for (int i = 0; i < object_state->node_count; i++)
			matrix4x3_interpolate(&object_state->previous_node_position[i], &current_object_nodes[i], Interpolation::GetInterpolateTime(), &object_state->interpolated_nodes[i]);

		*out_node_count = object_state->node_count;
		return object_state->interpolated_nodes;
	}

	int GetObjectInterpolateCount()
	{
		return object_count;
	}

	void Reset()
	{
		initialized = false;

		// int count = object_count;
		for (int i = 0; i < OBJECT_GAME_STATE_MAX_COUNT; i++)
		{
			ResetObject(i);
		}
	}

	void ApplyPatches()
	{
		PatchCall(Memory::GetAddressRelative(0x59650A), object_get_node_matrices_hook);
	}
}