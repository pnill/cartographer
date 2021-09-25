#include "Engine.h"

#include "Util/Hooks/Hook.h"

#include "Blam/Engine/Objects/Objects.h"

#include "H2MOD/Modules/Networking/Memory/bitstream.h"

#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"

namespace Engine::Objects
{
	//Grabs object from object table and verifies the type matches

	void __cdecl create_new_placement_data(s_object_placement_data* s_object_placement_data, datum object_definition_index, datum object_owner, int unk)
	{
		//LOG_TRACE_GAME("object_placement_data_new(s_object_placement_data: %08X,",s_object_placement_data);
		//LOG_TRACE_GAME("object_definition_index: %08X, object_owner: %08X, unk: %08X)", object_definition_index, object_owner, unk);

		typedef void(__cdecl object_placement_data_new)(void*, datum, datum, int);
		auto pobject_placement_data_new = Memory::GetAddress<object_placement_data_new*>(0x132163, 0x121033);

		pobject_placement_data_new(s_object_placement_data, object_definition_index, object_owner, unk);
	}

	//Pass new placement data into Create_object_new
	int __cdecl call_object_new(s_object_placement_data* pObject)
	{
		//LOG_TRACE_GAME("object_new(pObject: %08X)", pObject);

		typedef int(__cdecl object_new)(void*);
		auto p_object_new = Memory::GetAddress<object_new*>(0x136CA7, 0x125B77);

		return p_object_new(pObject);
	}

	//Pass datum from new object into object to sync
	bool __cdecl call_add_object_to_sync(datum gamestate_object_datum)
	{
		typedef int(__cdecl add_object_to_sync)(datum gamestate_object_datum);
		auto p_add_object_to_sync = Memory::GetAddress<add_object_to_sync*>(0x1B8D14, 0x1B2C44);

		return p_add_object_to_sync(gamestate_object_datum);
	}

#pragma region Biped variant patches
	void update_biped_object_variant_data(datum object_index, int variant_index)
	{
		s_biped_object_definition* biped_object = (s_biped_object_definition*)object_try_and_get_and_verify_type(object_index, FLAG(e_object_type::biped));
		// set this data only if we are dealing with a biped
		if (biped_object != NULL)
		{
			biped_object->variant_index = variant_index;
			//addDebugText("set the variant data to: %u for biped", variant_index);
		}
	}

	void __cdecl update_object_variant_index_hook(datum object_index, int variant_index)
	{
		auto p_resolve_variant_index_to_new_variant = Memory::GetAddressRelative<int(__cdecl*)(datum, int)>(0x52FE84, 0x51ED47);
		auto object = object_get_fast_unsafe<s_biped_object_definition>(object_index);

		object->model_variant_id = p_resolve_variant_index_to_new_variant(object_index, variant_index);

		// update the biped variant index
		update_biped_object_variant_data(object_index, variant_index);

		//addDebugText("object_index: %d, model_variant_id: %u", DATUM_ABSOLUTE_INDEX(object_index), object->variant_index);
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

	typedef void(__stdcall* c_simulation_unit_entity_definition_creation_encode)(void* thisptr, int creation_data_size, void* creation_data, int a3, bitstream* stream);
	c_simulation_unit_entity_definition_creation_encode pc_simulation_unit_entity_definition_encode;

	void __stdcall c_simulation_unit_entity_definition_creation_encode_hook(void* thisptr, int creation_data_size, void* creation_data, int a3, bitstream* stream)
	{
		int object_permutation_index = *(int*)((char*)creation_data + 36);
		stream->data_encode_integer("object-permutation-index", object_permutation_index, 32);

		//addDebugText("encoded entity creation: variant index: %d", object_permutation_index);
		pc_simulation_unit_entity_definition_encode(thisptr, creation_data_size, creation_data, a3, stream);
	}

	typedef bool(__stdcall* c_simulation_unit_entity_definition_creation_decode)(void* thisptr, int creation_data_size, void* creation_data, bitstream* stream);
	c_simulation_unit_entity_definition_creation_decode pc_simulation_unit_entity_definition_decode;

	bool __stdcall c_simulation_unit_entity_definition_creation_decode_hook(void* thisptr, int creation_data_size, void* creation_data, bitstream* stream)
	{
		int object_permutation_index = stream->data_decode_integer("object-permutation-index", 32);
		*(int*)((char*)creation_data + 36) = object_permutation_index;

		//addDebugText("decoded entity creation: variant index: %d", object_permutation_index);
		return pc_simulation_unit_entity_definition_decode(thisptr, creation_data_size, creation_data, stream);
	}

	int __stdcall c_simulation_object_entity_definition_object_create_object_hook(int thisx, void* creation_data, int a2, int a3, s_object_placement_data* object_placement_data)
	{
		// set the object placement data
		object_placement_data->variant_index = *(int*)((char*)creation_data + 36);
		//addDebugText("creating object with variant index: %d", object_placement_data->variant_index);
		return Memory::GetAddress<int(__thiscall*)(int, void*, int, int, s_object_placement_data*)>(0x1F32DB, 0x1DE374)(thisx, creation_data, a2, a3, object_placement_data);
	}

	// original update_object_variant_index is usercall, with data in CX register as first param
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

	typedef bool(__cdecl* tset_unit_color_data)(int, unsigned __int16, int a3);
	tset_unit_color_data pset_unit_color_data;

	bool __cdecl set_unit_color_data_hook(int a1, unsigned __int16 a2, int a3)
	{
		int object_creation_data = a1 - 0x10;
		int object_permutation_index = *(int*)((char*)object_creation_data + 36);

		if (object_permutation_index == 0)
			return pset_unit_color_data(a1, a2, a3);

		return 0;
	}

	void __stdcall object_build_creation_data_hook(datum object_index, void* object_creation_data)
	{
		auto p_object_build_creation_data = Memory::GetAddress<void(__stdcall*)(datum, void*)>(0x1F24ED, 0x1DD586);

		p_object_build_creation_data(object_index, object_creation_data);

		auto object = object_get_fast_unsafe<s_biped_object_definition>(object_index);

		*(int*)((BYTE*)object_creation_data + 0x24) = object->variant_index;

		//addDebugText("object build creation data: variant index: %d", object->variant_index);
	}

	void apply_biped_object_definition_patches()
	{
		// increase the data size for biped representation
		WriteValue<unsigned short>(Memory::GetAddressRelative(0x81E9A8, 0x7C1EB8) + 8, sizeof(s_biped_object_definition));

		// hook the function that updates the variant
		WriteJmpTo(Memory::GetAddressRelative(0x52FED3, 0x51ED96), update_object_variant_index_to_cdecl);

		// increase the size of the unit entity creation definition packet
		WriteValue<DWORD>(Memory::GetAddress(0x1F8028, 0x1E1D8E) + 1, 36 + 4);

		// increase the minimum required bitstream size
		WriteValue<BYTE>(Memory::GetAddressRelative(0x5F84B9, 0x5E221F) + 3, 5 + 32); // + 32 is quite much, but idk how bitstream will behave with less than 32 when dealin with integers

		PatchCall(Memory::GetAddressRelative(0x5F807A, 0x5E1DE0), object_build_creation_data_hook);

		//This encodes the unit creation packet, only gets executed on host.
		pc_simulation_unit_entity_definition_encode = (c_simulation_unit_entity_definition_creation_encode)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1F8503, 0x1E2269), (BYTE*)c_simulation_unit_entity_definition_creation_encode_hook, 10);

		//This decodes the unit creation packet, only gets executed on client.
		pc_simulation_unit_entity_definition_decode = (c_simulation_unit_entity_definition_creation_decode)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1F8557, 0x1E22BD), (BYTE*)c_simulation_unit_entity_definition_creation_decode_hook, 11);

		// Hooks a call within the creat_unit property on the client side in order to set their permutation index before spawn.
		PatchCall(Memory::GetAddress(0x1F9E6C, 0x1E3BD4), c_simulation_object_entity_definition_object_create_object_to_stdcall);

		// Hooks the part of the unit spawn from simulation that handles setting their color data in order to ensure AI do not have their color overridden
		PatchCall(Memory::GetAddress(0x1F9E34, 0x1E3B9C), set_unit_color_data_hook);
		pset_unit_color_data = Memory::GetAddress<tset_unit_color_data>(0x6E5C3, 0x6D1BF);
	}
#pragma endregion
}