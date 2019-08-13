#pragma once

#include "Blam\Cache\DataTypes\DataTypes.h"

// The game is using some sort of heap manager developed by Microsoft in 2000's named RockAll Heap Manager 

template<typename T = char*>
struct s_data_array
{
	char name[0x20]; //0x20
	int max_count; // 0x24
	int datum_size; // 0x28
	BYTE alignment_bit; // 0x29
	bool is_valid; // 0x2A
	WORD flags;  // 0x2C
	char data_signature[4]; // 0x30
	void **allocator; // 0x34
	struct {
		int next_index; // 0x38 
		int length; // 0x3C
	} active_indices; //0x3C
	int actual_count; //0x40 
	DatumIndex next_datum; //0x40
	T* data; //0x44
	int* data_usable_flags;
};
