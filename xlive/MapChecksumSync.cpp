#include "MapChecksumSync.h"
#include "stdafx.h"
#include "Hook.h"
#include "Util/hash.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

extern bool H2IsDediServer;
extern DWORD H2BaseAddr;

bool __cdecl compare_map_checksum(BYTE *checksum_data_a, BYTE *checksum_data_b)
{
	TRACE_FUNC(" Returning true");
	return true;
}

int __cdecl calc_map_checksum(HANDLE *file, int checksum_out)
{
	static int call_count = 0;
	if (call_count == 1) // logging ins't initialized yet on the first call for reasons
	{
		TRACE_FUNC("Calculating our checksums");
		MapChecksumSync::Calculate();
		TRACE_FUNC("Done checksumming");
	}
	else {
		TRACE_FUNC("Nop");
	}
	call_count++;
	return true;
}

static inline DWORD get_address(DWORD client, DWORD server = NULL)
{
	return H2BaseAddr + (H2IsDediServer ? server : client);
}

MapChecksumSync::state maps_status;
std::vector<HANDLE> locked_maps;
std::mutex map_mutex;

void unlock_all_maps()
{
	std::unique_lock<std::mutex> map_lock(map_mutex);
	for (HANDLE map : locked_maps)
	{
		CloseHandle(map);
	}
}

template<typename T>
bool set_contains_element(const std::unordered_set<T> &set, const T &element)
{
	for (const T &current_element : set)
	{
		if (current_element == element)
			return true;
	}
	return false;
}

void lock_file_write(const std::string &filename)
{
#ifdef _DEBUG
	TRACE_FUNC_N("Skipping locking in debug mode.");
	return;
#endif
	std::unique_lock<std::mutex> map_lock(map_mutex);
	HANDLE hfile = CreateFileA(filename.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (!LOG_CHECK(hfile != INVALID_HANDLE_VALUE))
	{
		TRACE_FUNC_N("Map '%s' couldn't be locked", filename.c_str());
		MapChecksumSync::StartupError("Failed to lock map file, close all map editing programs");
	}
}

void MapChecksumSync::StartupError(const std::string &error)
{
#ifdef _DEBUG
	TRACE_FUNC_N("error: '%s'", error.c_str());
	TRACE_FUNC_N("continuing despite error due to debug mode being enabled");
	return;
#endif
	// todo block further loading/mp here
	std::string error_message = error + " the game will now exit";
	MessageBoxA(NULL, error_message.c_str() , "Map checksumming failed!", S_OK);
	std::exit(-1);
}
void MapChecksumSync::Calculate()
{
	std::atexit(unlock_all_maps);
	std::at_quick_exit(unlock_all_maps);
	std::unordered_map<std::string, std::unordered_set<std::string>> builtin_maps
	{
		// mp maps
		{ "ascension", {"34a169cfed29f136b5031ca6ab45a0ca"} },
		{ "backwash", {"6ac88b64372c382ab4865e438810e8b3"} },
		{ "beavercreek", {"885d1d65d6b52079c1fc58876cffb00d"} },
		{ "burial_mounds", {"e1d0d14ce834b3503a4adae45dc1eff5"} },
		{ "coagulation", {"7bad598a5c5131f65c9c41cf45dc04c9"} },
		{ "colossus", {"f0c2a5d71c83776d556225f113e88cf1"} },
		{ "containment", {"e5600a8368f313bfc9c1e7ea23bf0dd7"} },
		{ "cyclotron", {"5fbb86545add866a4742acd81e3fcff6"} },
		{ "deltatap", {"57a78c6ed1bfdec074ddad232561a45e"} },
		{ "dune", {"3edaf7e620770ac85f7696754c0e0ab8"} },
		{ "elongation", {"af1d0f5ea2676279812bb878d244b30b"} },
		{ "foundation", {"d832ab4360cfb4edd7ce793e24c2fd45"} },
		{ "gemini", {"9e96550262279957b511984a55fa0a8f"} },
		{ "headlong", {"d318c1785cf661f09fbedb6c303effd5"} },
		{ "lockout", {"f8bed94c0e95d6272fc334605132bd43"} },
		{ "midship", {"918014dd0301b0d39a94c298fd240844"} },
		{ "needle", {"72e330f3e314ae7b41060de43b0076e9"} },
		{ "street_sweeper", {"d17bf6946a78324f92f849f6e0507838"} },
		{ "triplicate", {"b2ea1a841251148c21dbf2c4d5afa519"} },
		{ "turf", {"4d29ba793f77fc0304bd8bb9b83b0d79"} },
		{ "warlock", {"da533fc085996740e9fa50d83a37b7a3"} },
		{ "waterworks", {"bb0f5ab6bd17a7eb0e4f74f6e64a1b42"} },
		{ "zanzibar", {"286a7ae11e011a1d4a533f23a45ae494"} },

		{ "shared", {"64df15c07f2b1f28545774de18b7fee2"} },
	};
	maps_status.is_offical = true;
	for (auto ilter : builtin_maps)
	{
		std::string map_hash;
		std::string map_name = "maps\\" + ilter.first + ".map";
		lock_file_write(map_name); // lock map so no-one tries to edit it after we get the checksums
		if (hashes::calc_file_md5(map_name, map_hash))
		{
			maps_status.map_checksum_names[ilter.first] = map_hash;
			if (!set_contains_element(ilter.second, map_hash))
			{
				TRACE_FUNC_N("Map '%s' checksum doesn't match any offical checksum", ilter.first.c_str());
				if (ilter.first != "shared")
				{
					TRACE_FUNC_N("Consider moving modded map to custom maps folder and restoring default");
				}
				maps_status.is_offical = false;
			}
		} else {
			StartupError("See logs for details and contact support");
		}
	}
	for (auto ilter : maps_status.map_checksum_names)
	{
		TRACE_FUNC_N("%s : %s", ilter.first.c_str(), ilter.second.c_str());
	}
	TRACE_FUNC_N("is_offical = %s", maps_status.is_offical ? "true" : "false");
}

void MapChecksumSync::Init()
{
	TRACE_FUNC("Disabling map checksum");
	WriteJmpTo(get_address(0x8F914), compare_map_checksum);
	WriteJmpTo(get_address(0x8F664), calc_map_checksum);
}
