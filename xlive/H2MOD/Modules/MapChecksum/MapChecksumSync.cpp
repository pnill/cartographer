#include "stdafx.h"
#include "MapChecksumSync.h"
#include <unordered_set>
#include "Globals.h"
#include "Util/hash.h"
#include "H2MOD/Modules/Networking/NetworkSession/NetworkSession.h"

extern bool H2IsDediServer;
extern DWORD H2BaseAddr;
using namespace MapChecksumSync;
extern h2log *checksum_log;

#define CHECKSUM_LOG(msg, ...) \
	LOG_DEBUG_FUNC(msg, __VA_ARGS__); \
	LOG_DEBUG(checksum_log, msg, __VA_ARGS__);// \
	//if (H2IsDediServer)\
		//printf(msg "\n", __VA_ARGS__) // causes issues since the new logging was added


#pragma region patches
bool checksum_init_started = false;

bool __cdecl compare_map_checksum(BYTE *checksum_data_a, BYTE *checksum_data_b)
{
	if (!checksum_init_started)
		MapChecksumSync::Calculate();
	return true; // patch out check
}

bool __cdecl calc_map_checksum(HANDLE *file, BYTE *checksum_out)
{
	if (!checksum_init_started)
		MapChecksumSync::Calculate();
	if (file != nullptr) {
		// needed because the customs maps list uses the checksum to deduplicate the maps
		wchar_t buf[MAX_PATH];
		LOG_CHECK(GetFinalPathNameByHandleW(*file, buf, ARRAYSIZE(buf), VOLUME_NAME_DOS) != 0);
		LOG_TRACE_FUNCW(L"map name: {0:s}  handle: {1:p}", buf, (void*)file);
		size_t len = 0x20; // will only use 16 bytes
		memset(checksum_out, 0, len);
		if (!LOG_CHECK(hashes::calc_file_md5(buf, checksum_out, len, 0x2000))) { // only checksums header
			// generate some random data if we can't get an actual checksum
			XNetRandom(checksum_out, 0x20);
		}
		LOG_TRACE_FUNC("uuid {}", hashes::as_hex_string(checksum_out, 0x20));
	}
	return true;
}

static inline DWORD get_address(DWORD client, DWORD server = NULL)
{
	return H2BaseAddr + (H2IsDediServer ? server : client);
}
#pragma endregion

void MapChecksumSync::Init()
{
	LOG_TRACE_FUNC("Disabling map checksum");
	//WriteJmpTo(get_address(0x8F914, 0x81EBB), compare_map_checksum);
	//WriteJmpTo(get_address(0x8F664, 0x82171), calc_map_checksum);

	// hang on startup matters less and those functions seem to be called latter
	//if (H2IsDediServer)
	//	MapChecksumSync::Calculate();
}


MapChecksumSync::state maps_status;
std::vector<HANDLE> locked_maps;
std::mutex map_mutex;

#pragma region startup

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
	LOG_TRACE_FUNC("Skipping locking in debug mode.");
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
		CHECKSUM_LOG("Map '%s' couldn't be locked", filename);
		MapChecksumSync::StartupError("Failed to lock map file, close all map editing programs");
	}
}

bool startup_failure = false;
void MapChecksumSync::StartupError(const std::string &error)
{
#ifdef _DEBUG
	LOG_TRACE_FUNC("error: '{}'", error);
	LOG_TRACE_FUNC("continuing despite error due to debug mode being enabled");
	return;
#endif
	if (H2IsDediServer)
	{
		CHECKSUM_LOG("{}", error);
		std::exit(-1);
	}
	startup_failure = true;
	std::string error_message = error + " the game will now exit";
	run_async(
		MessageBoxA(NULL, error_message.c_str(), "Map checksumming failed!", S_OK);
		std::exit(-1);
	)
}

startup_state MapChecksumSync::get_startup_info()
{
	if (startup_failure)
		return startup_state::failed;
	if (maps_status.is_done)
		return startup_state::done;
	if (checksum_init_started)
		return startup_state::in_progress;
	return startup_state::not_done;
}

void MapChecksumSync::Calculate()
{
	CHECKSUM_LOG("Starting builtin map checksumming");
	checksum_init_started = true;
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

		// shared
		{ "shared", {"64df15c07f2b1f28545774de18b7fee2"} },
	};

	for (auto ilter : builtin_maps)
	{
		std::string map_hash;
		std::string map_name = "maps\\" + ilter.first + ".map";
		lock_file_write(map_name); // lock map so no-one tries to edit it after we get the checksums
		if (hashes::calc_file_md5(map_name, map_hash))
		{
			maps_status.map_checksum_list[ilter.first] = map_hash;
			if (!set_contains_element(ilter.second, map_hash))
			{
				CHECKSUM_LOG("Map '%s' checksum doesn't match any offical checksum", ilter.first);
				if (ilter.first != "shared")
				{
					CHECKSUM_LOG("Consider moving modded map to custom maps folder and restoring default");
				}
				maps_status.is_offical = false;
			}
		} else {
			MapChecksumSync::StartupError("See logs for details and contact support");
		}
	}
	for (auto ilter : maps_status.map_checksum_list)
	{
		CHECKSUM_LOG("%s : %s", ilter.first, ilter.second);
	}
	CHECKSUM_LOG("is_offical = %s", maps_status.is_offical ? "true" : "false");
	maps_status.is_done = true;
}

#pragma endregion

std::map<error_id, std::string> temp_type_mapping{
	{ error_id::diff_checksums, "Different checksums" },
	{ error_id::internal, "internal error" },
	{ error_id::official_needed, "official needed" },
	{ error_id::unofficial_needed, "unofficial needed" },
};


void MapChecksumSync::RuntimeError(error_id type)
{
	h2mod->exit_game();
	// maybe someone will make a nicer dialog
	run_async(
		MessageBoxA(NULL, temp_type_mapping[type].c_str(), "Map checksum error!", S_OK);
	)
}

#pragma region sync

void MapChecksumSync::SendState()
{
	LOG_TRACE_FUNC("NetworkSession::localPeerIsSessionHost() {}", NetworkSession::localPeerIsSessionHost());
	LOG_TRACE_FUNC("h2mod->GetEngineType() {}", h2mod->GetEngineType());
	if (!NetworkSession::localPeerIsSessionHost())
		return;
	H2ModPacket packet;
	packet.set_type(H2ModPacket::Type::H2ModPacket_Type_map_checksum_state_sync);

	h2mod_map_checksum_state *checksum_state = packet.mutable_checksum();
	checksum_state->set_is_offical(maps_status.is_offical);
	if (!maps_status.is_offical)
	{
		auto checksum_list = checksum_state->mutable_map_checksum_list();
		for (auto elem : maps_status.map_checksum_list)
		{
			StringMap_FieldEntry *proto_elem = checksum_list->Add();
			proto_elem->set_key(elem.first);
			proto_elem->set_value(elem.second);
		}
	}
//	network->send_h2mod_packet(packet);
}

void MapChecksumSync::HandlePacket(const H2ModPacket &packet)
{
	if (LOG_CHECK(packet.type() == H2ModPacket::Type::H2ModPacket_Type_map_checksum_state_sync && !NetworkSession::localPeerIsSessionHost() && packet.has_checksum()))
	{
		CHECKSUM_LOG("Processing packet");
		auto checksum_packet = packet.checksum();
		if (checksum_packet.is_offical() != maps_status.is_offical)
		{
			CHECKSUM_LOG("Server client is_offical mismatch client=%d, server=%d", maps_status.is_offical, checksum_packet.is_offical());
			MapChecksumSync::RuntimeError(maps_status.is_offical ? error_id::unofficial_needed : error_id::unofficial_needed);
			return;
		}

		if (!checksum_packet.is_offical())
		{
			CHECKSUM_LOG("Not offical comparing map checksums");
			if (checksum_packet.map_checksum_list_size() == 0)
			{
				CHECKSUM_LOG("Internal error checksum list is empty");
				RuntimeError(error_id::internal);
				return;
			}
			if (checksum_packet.map_checksum_list_size() != maps_status.map_checksum_list.size())
			{
				CHECKSUM_LOG("Internal error server checksum list length doesn't match client");
				RuntimeError(error_id::internal);
				return;
			}
			bool is_valid = true;
			auto server_list = checksum_packet.map_checksum_list();
			std::map<std::string, std::pair<std::string, std::string>> bad_maps_list;
			for (auto server_elem : server_list)
			{
				auto ilter = maps_status.map_checksum_list.find(server_elem.key());
				if (ilter == maps_status.map_checksum_list.end())
				{
					CHECKSUM_LOG("Internal error server has map client doesn't");
					RuntimeError(error_id::internal);
					return;
				}
				if (ilter->second != server_elem.value())
				{
					CHECKSUM_LOG("server client mismatch for map: %s, client: %s, server: %s", ilter->first, ilter->second, server_elem.value());
					bad_maps_list[ilter->first.c_str()] = { ilter->second, server_elem.value() };
					is_valid = false;
				}
			}
			if (!is_valid) {
				RuntimeError(error_id::diff_checksums);
				return;
			}
		}
	}
}
#pragma endregion
