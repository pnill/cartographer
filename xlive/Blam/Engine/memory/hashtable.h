#pragma once

struct s_hash_table
{
	char name[32];
	int32 element_count;
	int32 max_count;
	int32 key_size;
	int32(__cdecl* hash_proc)(const void*);
	bool(__cdecl* compare_proc)(const void*, const void*);
	void* allocator;
	void* elements;
	int32* data;
};
ASSERT_STRUCT_SIZE(s_hash_table, 64);