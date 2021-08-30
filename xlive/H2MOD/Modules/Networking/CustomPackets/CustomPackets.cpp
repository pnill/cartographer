#include "stdafx.h"
#include "CustomPackets.h"

#include "Util\Hooks\Hook.h"
#include "..\Memory\bitstream.h"
#include "..\..\MapManager\MapManager.h"
#include "H2MOD/Modules/EventHandler/EventHandler.h"
#include "H2MOD/Modules/Config/Config.h"

char g_network_message_types[e_network_message_types::end * 32];

void register_packet_impl(void *packetObject, int type, char* name, int a4, int size1, int size2, void* write_packet_method, void* read_packet_method, void* unk_callback)
{
	typedef void(__thiscall* register_packet_type)(void *, int, char*, int, int, int, void*, void*, void*);
	auto register_packet = reinterpret_cast<register_packet_type>(Memory::GetAddress(0x1E81D6, 0x1CA199));
	return register_packet(packetObject, type, name, a4, size1, size2, write_packet_method, read_packet_method, unk_callback);
}

const char* getNetworkMessageName(int enumVal) 
{
	return network_message_name[enumVal];
}

void __cdecl encode_map_file_name_packet(bitstream* stream, int a2, s_custom_map_filename* data)
{
	stream->data_encode_string_wide("map-file-name", &data->file_name, ARRAYSIZE(data->file_name));
	stream->data_encode_integer("map-download-id", data->map_download_id, CHAR_BIT * sizeof(data->map_download_id));
}
bool __cdecl decode_map_file_name_packet(bitstream* stream, int a2, s_custom_map_filename* data)
{
	stream->data_decode_string_wide("map-file-name", &data->file_name, ARRAYSIZE(data->file_name));
	data->map_download_id = stream->data_decode_integer("map-download-id", CHAR_BIT * sizeof(data->map_download_id));
	return stream->packet_is_valid() == false;
}

void __cdecl encode_request_map_filename_packet(bitstream* stream, int a2, s_request_map_filename* data)
{
	stream->data_encode_bits("user-identifier", &data->user_identifier, player_identifier_size_bits);
	stream->data_encode_integer("map-download-id", data->map_download_id, CHAR_BIT * sizeof(data->map_download_id));
}
bool __cdecl decode_request_map_filename_packet(bitstream* stream, int a2, s_request_map_filename* data)
{
	stream->data_decode_bits("user-identifier", &data->user_identifier, player_identifier_size_bits);
	data->map_download_id = stream->data_decode_integer("map-download-id", CHAR_BIT * sizeof(data->map_download_id));
	return stream->packet_is_valid() == false;
}

void __cdecl encode_team_change_packet(bitstream* stream, int a2, s_team_change* data)
{
	stream->data_encode_integer("team-index", data->team_index, 32);
}
bool __cdecl decode_team_change_packet(bitstream* stream, int a2, s_team_change* data)
{
	data->team_index = stream->data_decode_integer("team-index", 32);
	return stream->packet_is_valid() == false;
}

void __cdecl encode_rank_change_packet(bitstream* stream, int a2, s_rank_change* data)
{
	stream->data_encode_integer("rank", data->rank, 8);
}

bool __cdecl decode_rank_change_packet(bitstream* stream, int a2, s_rank_change* data)
{
	data->rank = stream->data_decode_integer("rank", 8);
	return stream->packet_is_valid() == false;
}

void __cdecl encode_anti_cheat_packet(bitstream* stream, int a2, s_anti_cheat* data)
{
	stream->data_encode_bool("anti-cheat-enabled", data->enabled);
}
bool __cdecl decode_anti_cheat_packet(bitstream* stream, int a2, s_anti_cheat* data)
{
	data->enabled = stream->data_decode_bool("anti-cheat-enabled");
	return stream->packet_is_valid() == false;
}

void register_custom_packets(void* network_messages)
{
	typedef void(__cdecl* register_test_packet)(void* network_messages);
	auto p_register_test_packet = Memory::GetAddress<register_test_packet>(0x1ECE05, 0x1CD7BE);

	p_register_test_packet(network_messages);

	register_packet_impl(network_messages, request_map_filename, "request-map-filename", 0, sizeof(s_request_map_filename), sizeof(s_request_map_filename),
		(void*)encode_request_map_filename_packet, (void*)decode_request_map_filename_packet, NULL);

	register_packet_impl(network_messages, custom_map_filename, "map-file-name", 0, sizeof(s_custom_map_filename), sizeof(s_custom_map_filename),
		(void*)encode_map_file_name_packet, (void*)decode_map_file_name_packet, NULL);

	register_packet_impl(network_messages, team_change, "team-change", 0, sizeof(s_team_change), sizeof(s_team_change),
		(void*)encode_team_change_packet, (void*)decode_team_change_packet, NULL);

	register_packet_impl(network_messages, rank_change, "rank-change", 0, sizeof(s_rank_change), sizeof(s_rank_change),
		(void*)encode_rank_change_packet, (void*)decode_rank_change_packet, NULL);

	register_packet_impl(network_messages, anti_cheat, "anti-cheat", 0, sizeof(s_anti_cheat), sizeof(s_anti_cheat),
		(void*)encode_anti_cheat_packet, (void*)decode_anti_cheat_packet, NULL);
}

typedef void(__stdcall *handle_out_of_band_message)(void *thisx, network_address* address, int message_type, int a4, void* packet);
handle_out_of_band_message p_handle_out_of_band_message;

void __stdcall handle_out_of_band_message_hook(void *thisx, network_address* address, int message_type, int a4, void* packet)
{
	/*
		This handles received out-of-band data
	*/

	/* since the update 0.5.4.0 this doesn't work properly anymore, the update known to work is 0.5.3.0 */
	/* swithced to in-band-data protocol */
	/*switch (message_type)
	{
	case request_map_filename:
	{
		s_request_map_filename* received_data = (s_request_map_filename*)packet;
		signed int peer_index = NetworkSession::getPeerIndexFromNetworkAddress(address);
		LOG_TRACE_NETWORK("[H2MOD-CustomPackets] received on handle_out_of_band_message request-map-filename from XUID: {}, peer index: {}", received_data->user_identifier, peer_index);
		network_session* session = NetworkSession::getCurrentNetworkSession();

		/ * We will use in-band data, because with the latest update there is an issue where peers don't send the map name request packet when another peer joined the session * /
		/ * I think the problem is with the new XLive netcode, though it's weird the other out-of-band game packets work just fine * /

		if (peer_index != -1 && peer_index != session->local_peer_index)
		{
			s_custom_map_filename data;
			SecureZeroMemory(&data, sizeof(s_custom_map_filename));

			std::wstring map_filename;
			mapManager->getMapFilename(map_filename);
			data.is_custom_map = false;
			if (map_filename.size() > 0)
			{
				data.is_custom_map = true;
				wcsncpy_s(data.file_name, map_filename.c_str(), ARRAYSIZE(data.file_name));
			}

			LOG_TRACE_NETWORK(L"[H2MOD-CustomPackets] sending map file name packet to XUID: {}, peer index: {}, map name: {}", received_data->user_identifier, peer_index, map_filename.c_str());
			observer_channel_send_message(session->network_observer_ptr, session->unk_index, session->peer_observer_channels[peer_index].observer_index, network_observer::network_message_send_protocol::out_of_band,
				map_file_name, sizeof(s_custom_map_filename), &data);
		}

		return;
	}

	case map_file_name:
	{
		s_custom_map_filename* received_data = (s_custom_map_filename*)packet;
		if (received_data->is_custom_map)
		{
			std::wstring filename_wstr(received_data->file_name);
			std::string filename_str(filename_wstr.begin(), filename_wstr.end());
			mapManager->setMapFileNameToDownload(filename_str);
			LOG_TRACE_NETWORK(L"[H2MOD-CustomPackets] received on handle_out_of_band_message map_file_name: {}", received_data->file_name);
		}

		return;
	}

	case team_change:
	{
		s_team_change* received_data = (s_team_change*)packet;
		h2mod->set_local_team_index(0, received_data->team_index);
		return;
	}

	case unit_grenades:
	{
		s_unit_grenades* received_data = (s_unit_grenades*)packet;
		if (!Memory::isDedicatedServer())
			h2mod->set_local_player_unit_grenades(received_data->type, received_data->count, received_data->player_index);
		return;
	}

	default:
			break;
	}*/

	/* surprisingly the game doesn't use this too much, pretty much for request-join and tme-sync packets */
	LOG_TRACE_NETWORK("handle_out_of_band_message_hook() - Received message: {} from peer index: {}", getNetworkMessageName(message_type), NetworkSession::getPeerIndexFromNetworkAddress(address));

	p_handle_out_of_band_message(thisx, address, message_type, a4, packet);
}

typedef void(__stdcall *handle_channel_message)(void *thisx, int network_channel_index, int message_type, int dynamic_data_size, void* packet);
handle_channel_message p_handle_channel_message;

void __stdcall handle_channel_message_hook(void *thisx, int network_channel_index, int message_type, int dynamic_data_size, void* packet)
{
	/*
		This handles received in-band data
	*/

	network_address addr;
	ZeroMemory(&addr, sizeof(network_address));
	network_channel* peer_network_channel = network_channel::getNetworkChannel(network_channel_index);

	switch (message_type)
	{
	case request_map_filename:
	{
		s_request_map_filename* received_data = (s_request_map_filename*)packet;
		LOG_DEBUG_NETWORK("[H2MOD-CustomPackets] received on handle_channel_message_hook request-map-filename from XUID: {}", received_data->user_identifier);
		if (peer_network_channel->channel_state == network_channel::e_channel_state::unk_state_5
			&& peer_network_channel->getNetworkAddressFromNetworkChannel(&addr))
		{
			LOG_TRACE_NETWORK("  - network address: {:x}", ntohl(addr.address.ipv4));

			int peer_index = NetworkSession::getPeerIndexFromNetworkAddress(&addr);
			network_session* session = NetworkSession::getCurrentNetworkSession();
			if (peer_index != -1 && peer_index != session->local_peer_index)
			{
				s_custom_map_filename data;
				SecureZeroMemory(&data, sizeof(s_custom_map_filename));

				std::wstring map_filename;
				mapManager->getMapFilename(map_filename);
				if (!map_filename.empty())
				{
					wcsncpy_s(data.file_name, map_filename.c_str(), map_filename.length());
					data.map_download_id = received_data->map_download_id;

					LOG_TRACE_NETWORK(L"[H2MOD-CustomPackets] sending map file name packet to XUID: {}, peer index: {}, map name: {}, download id {}", 
						received_data->user_identifier, 
						peer_index, map_filename.c_str(), received_data->map_download_id);

					network_observer* observer = session->network_observer_ptr;
					peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(peer_index);

					observer->sendNetworkMessage(session->session_index, observer_channel->observer_index, network_observer::e_network_message_send_protocol::in_band, custom_map_filename, sizeof(s_custom_map_filename), &data);
				}
				else
				{
					LOG_TRACE_NETWORK(L"[H2MOD-CustomPackets] no map file name found, abort sending packet! {} - {}", received_data->user_identifier, peer_index, map_filename.c_str());
				}
			}
		}
		return;
	}

	case custom_map_filename:
	{
		if (peer_network_channel->channel_state == network_channel::e_channel_state::unk_state_5)
		{
			s_custom_map_filename* received_data = (s_custom_map_filename*)packet;
			if (received_data->map_download_id != NONE)
			{
				auto map_download_query = mapManager->getDownloadQueryById(received_data->map_download_id);
				if (map_download_query != nullptr)
				{
					map_download_query->SetMapNameToDownload(received_data->file_name);
					LOG_DEBUG_NETWORK(L"[H2MOD-CustomPackets] received on handle_channel_message_hook custom_map_filename: {}", received_data->file_name);
				}
				else
				{
					// unlikely
					LOG_TRACE_NETWORK("[H2MOD-CustomPackets] - query with id {:X} hasn't been found!", received_data->map_download_id);
					return;
				}
			}

			LOG_TRACE_NETWORK(L"[H2MOD-CustomPackets] - received map name: {}, no download ID", received_data->file_name);
		}
		return;
	}

	case team_change:
	{
		if (peer_network_channel->channel_state == network_channel::e_channel_state::unk_state_5)
		{
			s_team_change* received_data = (s_team_change*)packet;
			LOG_DEBUG_NETWORK(L"[H2MOD-CustomPackets] recieved on handle_channel_message_hook team_change: {}", received_data->team_index);
			h2mod->set_local_team_index(0, received_data->team_index);
		}
		return;
	}

	case rank_change:
	{
		if (peer_network_channel->channel_state == network_channel::e_channel_state::unk_state_5)
		{
			s_rank_change* recieved_data = (s_rank_change*)packet;
			LOG_DEBUG_NETWORK(L"H2MOD-CustomPackets] recieved on handle_channel_message_hook rank_change: {}", recieved_data->rank);
			h2mod->set_local_rank(recieved_data->rank);
		}
		return;
	}

	case anti_cheat:
	{
		if (peer_network_channel->channel_state == network_channel::e_channel_state::unk_state_5)
		{
			s_anti_cheat* recieved_data = (s_anti_cheat*)packet;
			H2Config_anti_cheat_enabled = recieved_data->enabled;
		}
		return;
	}
	
	// default packet
	case leave_session:
	{
		if (peer_network_channel->channel_state == network_channel::e_channel_state::unk_state_5
			&& peer_network_channel->getNetworkAddressFromNetworkChannel(&addr))
		{
			auto peer_index = NetworkSession::getPeerIndexFromNetworkAddress(&addr);
			EventHandler::executeNetworkPlayerRemoveCallbacks(peer_index);
		}
		break; // don't return, leave the game to update state
	}

	//case parameters_update:
	//{
	//	if (peer_network_channel->channel_state == network_channel::e_channel_state::unk_state_5
	//		&& peer_network_channel->getNetworkAddressFromNetworkChannel(&addr))
	//	{
	//		auto peer_index = NetworkSession::getPeerIndexFromNetworkAddress(&addr);
	//		//LOG_INFO_NETWORK(L"Got a parameters update from {} : {}", NetworkSession::getPeerPlayerName(peer_index), std::to_wstring(NetworkSession::getCurrentNetworkSession()->parameters.dedicated_server));
	//		if (peer_index == NetworkSession::getCurrentNetworkSession()->session_host_peer_index && !NetworkSession::getCurrentNetworkSession()->parameters.dedicated_server)
	//		{
	//			const auto host_xuid = NetworkSession::getPeerXUID(peer_index);
	//			if (host_xuid != NONE) {
	//				//LOG_TRACE_NETWORK(L"Setting up team persistance with host xuid {}", IntToWString<XUID>(host_xuid, std::dec));
	//				h2mod->set_local_clan_tag(0, host_xuid);
	//				//EventHandler::registerGameStateCallback({
	//				//		"UnPersistHostTeam1",
	//				//		game_life_cycle::life_cycle_in_game,
	//				//		[]()
	//				//		{
	//				//			//LOG_TRACE_NETWORK(L"Removing Persistance to previous host");
	//				//			h2mod->set_local_clan_tag(0, 0);
	//				//		}, true
	//				//	}, false);
	//				EventHandler::registerGameStateCallback({
	//						"UnPersistHostTeam2",
	//						game_life_cycle::life_cycle_none,
	//						[]()
	//						{
	//							//LOG_TRACE_NETWORK(L"Removing Persistance to previous host");
	//							h2mod->set_local_clan_tag(0, 0);
	//						}, true
	//					}, false);
	//			}
	//		}
	//	}
	//}

	default:
		break;
	} // switch (message_type)

	if (peer_network_channel->getNetworkAddressFromNetworkChannel(&addr)) 
	{
		LOG_TRACE_NETWORK("handle_channel_message_hook() - Received message: {} from peer index: {}, address: {:x}", getNetworkMessageName(message_type), NetworkSession::getPeerIndexFromNetworkAddress(&addr), ntohl(addr.address.ipv4));
	}
	else
	{
		LOG_ERROR_NETWORK("handle_channel_message_hook() - Received message: {} from network channel: {} that maybe shouldn't have been received", getNetworkMessageName(message_type), network_channel_index);
	}


	p_handle_channel_message(thisx, network_channel_index, message_type, dynamic_data_size, packet);

	if (message_type == player_add) {
		if (peer_network_channel->channel_state == network_channel::e_channel_state::unk_state_5
			&& peer_network_channel->getNetworkAddressFromNetworkChannel(&addr))
		{
			auto peer_index = NetworkSession::getPeerIndexFromNetworkAddress(&addr);
			EventHandler::executeNetworkPlayerAddCallbacks(peer_index);
			CustomPackets::sendAntiCheat(peer_index);
		}
	}
}

void CustomPackets::sendRequestMapFilename(int mapDownloadId)
{
	network_session* session = NetworkSession::getCurrentNetworkSession();

	if (session->local_session_state == _network_session_state_peer_established)
	{
		s_request_map_filename data;
		XUserGetXUID(0, &data.user_identifier);
		data.map_download_id = mapDownloadId;

		network_observer* observer = session->network_observer_ptr;
		peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(session->session_host_peer_index);

		if (observer_channel->field_1) {
			observer->sendNetworkMessage(session->session_index, observer_channel->observer_index, network_observer::e_network_message_send_protocol::in_band, request_map_filename, sizeof(s_request_map_filename), &data);

			/*LOG_TRACE_NETWORK("[H2MOD-CustomPackets] Sending map name request info: session host peer index: {}, observer index {}, observer bool unk: {}, session index: {}",
				session->session_host_peer_index,
				observer_channel->observer_index,
				observer_channel->field_1,
				session->session_index);*/
		}
	}
}

void CustomPackets::sendTeamChange(int peerIndex, int teamIndex)
{
	network_session* session = NetworkSession::getCurrentNetworkSession();
	if (NetworkSession::localPeerIsSessionHost())
	{
		s_team_change data;
		data.team_index = teamIndex;

		network_observer* observer = session->network_observer_ptr;
		peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(peerIndex);

		if (peerIndex != -1 && peerIndex != session->local_peer_index)
		{
			if (observer_channel->field_1) {
				observer->sendNetworkMessage(session->session_index, observer_channel->observer_index, network_observer::e_network_message_send_protocol::in_band, team_change, sizeof(s_team_change), &data);
			}
		}
	}
}

void CustomPackets::sendRankChange(int peerIndex, byte rank)
{
	network_session* session = NetworkSession::getCurrentNetworkSession();
	if (NetworkSession::localPeerIsSessionHost())
	{
		s_rank_change data;
		data.rank = rank;

		network_observer* observer = session->network_observer_ptr;
		peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(peerIndex);

		if (peerIndex != -1 && peerIndex != session->local_peer_index)
		{
			if (observer_channel->field_1) {
				observer->sendNetworkMessage(session->session_index, observer_channel->observer_index, network_observer::e_network_message_send_protocol::in_band, rank_change, sizeof(s_rank_change), &data);
			}
		}
	}
}
void CustomPackets::sendAntiCheat(int peerIndex)
{
	network_session* session = NetworkSession::getCurrentNetworkSession();

	if (NetworkSession::localPeerIsSessionHost())
	{

		network_observer* observer = session->network_observer_ptr;
		peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(peerIndex);

		s_anti_cheat data;
		data.enabled = H2Config_anti_cheat_enabled;
		if (observer_channel->field_1) {
			observer->sendNetworkMessage(session->session_index, observer_channel->observer_index, network_observer::e_network_message_send_protocol::in_band, anti_cheat, sizeof(s_anti_cheat), &data);
		}
	}
}

void CustomPackets::ApplyGamePatches()
{
	WritePointer(Memory::GetAddress(0x1AC733, 0x1AC901), g_network_message_types);
	WritePointer(Memory::GetAddress(0x1AC8F8, 0x1ACAC6), g_network_message_types);
	WriteValue<BYTE>(Memory::GetAddress(0x1E825E, 0x1CA221), e_network_message_types::end);
	WriteValue<int>(Memory::GetAddress(0x1E81C6, 0x1CA189), e_network_message_types::end * 32);

	PatchCall(Memory::GetAddress(0x1B5196, 0x1A8EF4), register_custom_packets);

	p_handle_out_of_band_message = (handle_out_of_band_message)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1E907B, 0x1CB03B), (BYTE*)handle_out_of_band_message_hook, 8);
	p_handle_channel_message = (handle_channel_message)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1E929C, 0x1CB25C), (BYTE*)handle_channel_message_hook, 8);
}