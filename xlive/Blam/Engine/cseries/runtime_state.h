#pragma once

#define k_runtime_state_size 204800

struct runtime_state_globals
{
	bool locked;
	void* base_address; // pointer to the allocation of k_runtime_state_size
	int32 allocated_size;
	int32 unk_C;
	int32 unk_10;
};
CHECK_STRUCT_SIZE(runtime_state_globals, 0x14);