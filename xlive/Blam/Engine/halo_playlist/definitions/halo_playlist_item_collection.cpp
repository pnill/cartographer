#include "stdafx.h"
#include "halo_playlist_item_collection.h"

wchar_t* halo_playlist_item_collection_get_name(s_halo_playlist_item_collection* collection, int32 value)
{
	if (collection->count <= 0)
		return L"";

	for(uint32 i = 0; i < collection->count; ++i)
		if (collection->items[i].value == value)
			return collection->items[i].name;
	
	return L"";
}

int32 halo_playlist_item_collection_get_value(s_halo_playlist_item_collection* collection, wchar_t* value)
{
	return INVOKE_TYPE(0, 0xF1E9, int32(*)(s_halo_playlist_item_collection*, wchar_t*), collection, value);
}

bool halo_playlist_item_collection_get_boolean_value(wchar_t* value, bool* out_result)
{
	switch(*value)
	{
		case L'0':
		case L'F':
		case L'N':
		case L'f':
		case L'n':
		{
			*out_result = false;
			return true;
		}
		case L'1':
		case L'T':
		case L'Y':
		case L't':
		case L'y':
		{
			*out_result = true;
			return true;
		}
		default:
		{
			if (_wcsicmp(value, L"on") == 0)
			{
				*out_result = true;
				return true;
			}
			if (_wcsicmp(value, L"off") == 0)
			{
				*out_result = false;
				return true;
			}

			return false;
		}
	}
}

int32 halo_playlist_item_collection_get_int_time_value(wchar_t* value)
{
	s_halo_playlist_item_collection* integer_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355290);
	return halo_playlist_item_collection_get_value(integer_item_collection, value);
}

int32 halo_playlist_item_collection_get_int_value(wchar_t* value)
{
	s_halo_playlist_item_collection* integer_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355210);
	return halo_playlist_item_collection_get_value(integer_item_collection, value);
}

int16 halo_playlist_item_collection_player_count_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* player_count_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355230);
	return (int16)halo_playlist_item_collection_get_value(player_count_item_collection, value);
}

