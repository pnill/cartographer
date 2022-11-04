#include "stdafx.h"

#include "Objects.h"

#include "Util\Hooks\Hook.h"

#include "Blam\Engine\Memory\bitstream.h"
#include "Blam\Engine\Objects\Objects.h"
#include "Blam\Engine\Players\Players.h"
#include "Blam\Engine\Simulation\GameInterface\SimulationGameUnits.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\PlayerRepresentation\PlayerRepresentation.h"

namespace Engine::Objects
{
	void create_new_placement_data(s_object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, int a4)
	{
		LOG_TRACE_GAME("{}: {:X}, object_owner: {:X}, unk: {:X})", __FUNCTION__, object_definition_idx, object_owner_idx, a4);

		typedef void(__cdecl* object_placement_data_new_t)(void*, datum, datum, int);
		auto p_object_placement_data_new = Memory::GetAddress<object_placement_data_new_t>(0x132163, 0x121033);

		p_object_placement_data_new(object_placement_data, object_definition_idx, object_owner_idx, a4);
	}

	//Pass new placement data into Create_object_new
	datum object_new(s_object_placement_data* object_placement_data)
	{
		LOG_TRACE_GAME("{}", __FUNCTION__);

		typedef datum(__cdecl* object_new_t)(s_object_placement_data*);
		auto p_object_new = Memory::GetAddress<object_new_t>(0x136CA7, 0x125B77);

		return p_object_new(object_placement_data);
	}

	//Pass datum from new object into object to sync
	void simulation_action_object_create(datum object_idx)
	{
		typedef void(__cdecl* simulation_action_object_create_t)(datum);
		auto p_simulation_action_object_create = Memory::GetAddress<simulation_action_object_create_t>(0x1B8D14, 0x1B2C44);

		return p_simulation_action_object_create(object_idx);
	}

	void object_destroy(datum object_idx)
	{
		typedef void(__cdecl object_destroy_t)(datum);
		auto p_object_destroy = Memory::GetAddress<object_destroy_t*>(0x136005);

		p_object_destroy(object_idx);
	}

#pragma region Biped variant patches
	void update_biped_object_variant_data(datum object_idx, int variant_index)
	{
		s_biped_data_definition* biped_object = (s_biped_data_definition*)object_try_and_get_and_verify_type(object_idx, FLAG(e_object_type::biped));
		// set this data only if we are dealing with a biped
		if (biped_object != NULL)
		{
			biped_object->variant_name = variant_index;
			// addDebugText("set the variant data to: %u for biped", variant_index);
		}
	}

	void __cdecl update_object_variant_index_hook(datum object_idx, int variant_index)
	{
		auto p_resolve_variant_index_to_new_variant = Memory::GetAddressRelative<int(__cdecl*)(datum, int)>(0x52FE84, 0x51ED47);
		auto object = object_get_fast_unsafe<s_biped_data_definition>(object_idx);

		object->model_variant_id = p_resolve_variant_index_to_new_variant(object_idx, variant_index);
		// update the biped variant index
		update_biped_object_variant_data(object_idx, variant_index);

		//addDebugText("object_index: %d, model_variant_id: %u", DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index), object->variant_index);
	}

	// original update_object_variant_index is usercall, with data in CX register as first param
	__declspec(naked) void update_object_variant_index_to_cdecl()
	{
		__asm
		{
			// we don't preserve eax, because it's a volatile register thats expected to have 
			// it's value changed

			mov eax, [esp + 4] // copy the variant index
			push eax		   // then push to stack for update_object_variant_index_hook as second param
			push ecx		   // ecx stores the object index (in CX register size), push as first param
			call update_object_variant_index_hook
			add esp, 8			// reset stack after update_object_variant_index_hook
			retn				// return to procedure
		}
	}

	typedef void(__stdcall* c_simulation_unit_entity_definition_creation_encode_t)(void* thisptr, int creation_data_size, void* creation_data, int a3, bitstream* stream);
	c_simulation_unit_entity_definition_creation_encode_t p_c_simulation_unit_entity_definition_encode;

	void __stdcall c_simulation_unit_entity_definition_creation_encode_hook(void* thisptr, int creation_data_size, s_simulation_unit_creation_data* creation_data, int a3, bitstream* stream)
	{
		int object_permutation_idx = creation_data->variant_name.get_packed();
		stream->data_encode_integer("object-permutation-index", object_permutation_idx, 32);
		//addDebugText("encoded entity creation: variant index: %d", object_permutation_index);
		p_c_simulation_unit_entity_definition_encode(thisptr, creation_data_size, creation_data, a3, stream);
	}

	typedef bool(__stdcall* c_simulation_unit_entity_definition_creation_decode_t)(void* thisptr, int creation_data_size, void* creation_data, bitstream* stream);
	c_simulation_unit_entity_definition_creation_decode_t p_c_simulation_unit_entity_definition_decode;

	bool __stdcall c_simulation_unit_entity_definition_creation_decode_hook(void* thisptr, int creation_data_size, s_simulation_unit_creation_data* creation_data, bitstream* stream)
	{
		int object_permutation_idx = stream->data_decode_integer("object-permutation-index", 32);
		creation_data->variant_name = object_permutation_idx;
		//addDebugText("decoded entity creation: variant index: %d", object_permutation_index);
		return p_c_simulation_unit_entity_definition_decode(thisptr, creation_data_size, creation_data, stream);
	}

	int __stdcall c_simulation_object_entity_definition_object_create_object_hook(int thisx, s_simulation_unit_creation_data* creation_data, int a2, int a3, s_object_placement_data* object_placement_data)
	{
		// set the object placement data
		object_placement_data->variant_name = creation_data->variant_name;
		if(*(byte*)((char*)creation_data + 0x10) != -1)
		{
			auto profile = reinterpret_cast<s_player::s_player_properties::s_player_profile*>((char*)creation_data + 0x10);
			object_placement_data->tag_index = PlayerRepresentation::get_object_datum_from_representation(profile->player_character_type);
		}
		//addDebugText("creating object with variant index: %d", object_placement_data->variant_name);
		return Memory::GetAddress<int(__thiscall*)(int, void*, int, int, s_object_placement_data*)>(0x1F32DB, 0x1DE374)(thisx, creation_data, a2, a3, object_placement_data);
	}

	__declspec(naked) void c_simulation_object_entity_definition_object_create_object_to_stdcall()
	{
		__asm
		{
			pop eax // pop return address
			push ecx // push ecx as first param
			push eax // push the return address back on stack
			jmp c_simulation_object_entity_definition_object_create_object_hook
		}
	}

	typedef bool(__cdecl* set_unit_color_data_t)(int, unsigned __int16, int a3);
	set_unit_color_data_t p_set_unit_color_data;

	bool __cdecl set_unit_color_data_hook(int a1, unsigned __int16 a2, int a3)
	{
		auto object_creation_data = (s_simulation_unit_creation_data*)(a1 - 0x10);
		int object_permutation_idx = object_creation_data->variant_name.get_packed();

		//if (object_permutation_index == 0)
		if(*(byte*)((char*)a1) != -1)
			return p_set_unit_color_data(a1, a2, a3);

		return 0;
	}

	void __stdcall object_build_creation_data_hook(datum object_idx, s_simulation_unit_creation_data* object_creation_data)
	{
		auto p_object_build_creation_data = Memory::GetAddress<void(__stdcall*)(datum, void*)>(0x1F24ED, 0x1DD586);

		p_object_build_creation_data(object_idx, object_creation_data);

		auto object = object_get_fast_unsafe<s_biped_data_definition>(object_idx);

		object_creation_data->variant_name = object->variant_name;

		//addDebugText("object build creation data: variant index: %d", object->variant_index);
	}

	void apply_biped_object_definition_patches()
	{
		LOG_INFO_GAME("{} - applying hooks", __FUNCTION__);

		// increase the data size for biped representation
		WriteValue<unsigned short>(Memory::GetAddressRelative(0x81E9A8, 0x7C1EB8) + 8, sizeof(s_biped_data_definition));

		// hook the function that updates the variant
		WriteJmpTo(Memory::GetAddressRelative(0x52FED3, 0x51ED96), update_object_variant_index_to_cdecl);

		// increase the size of the unit entity creation definition packet
		WriteValue<DWORD>(Memory::GetAddress(0x1F8028, 0x1E1D8E) + 1, sizeof(s_simulation_unit_creation_data));

		// increase the minimum required bitstream size
		WriteValue<BYTE>(Memory::GetAddressRelative(0x5F84B9, 0x5E221F) + 3, 5 + 32); // + 32 is quite much, but idk how bitstream will behave with less than 32 when dealin with integers

		PatchCall(Memory::GetAddressRelative(0x5F807A, 0x5E1DE0), object_build_creation_data_hook);

		//This encodes the unit creation packet, only gets executed on host.
		p_c_simulation_unit_entity_definition_encode = (c_simulation_unit_entity_definition_creation_encode_t)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1F8503, 0x1E2269), (BYTE*)c_simulation_unit_entity_definition_creation_encode_hook, 10);

		//This decodes the unit creation packet, only gets executed on client.
		p_c_simulation_unit_entity_definition_decode = (c_simulation_unit_entity_definition_creation_decode_t)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1F8557, 0x1E22BD), (BYTE*)c_simulation_unit_entity_definition_creation_decode_hook, 11);

		// Hooks a call within the creat_unit property on the client side in order to set their permutation index before spawn.
		PatchCall(Memory::GetAddress(0x1F9E6C, 0x1E3BD4), c_simulation_object_entity_definition_object_create_object_to_stdcall);

		// Hooks the part of the unit spawn from simulation that handles setting their color data in order to ensure AI do not have their color overridden
		PatchCall(Memory::GetAddress(0x1F9E34, 0x1E3B9C), set_unit_color_data_hook);
		p_set_unit_color_data = Memory::GetAddress<set_unit_color_data_t>(0x6E5C3, 0x6D1BF);
	}

	int object_get_count()
	{
		s_data_iterator object_it(get_objects_header());
		return object_it.get_data_count();
	}

	int object_count_from_iter()
	{
		s_data_iterator object_it(get_objects_header());
		int count = 0;
		while (object_it.get_next_datum())
		{
			count++;
		}
		return count;
	}
#pragma endregion
}
