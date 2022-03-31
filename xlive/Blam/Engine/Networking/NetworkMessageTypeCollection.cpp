#include "stdafx.h"

#include "NetworkMessageTypeCollection.h"
#include "Blam\Engine\Memory\bitstream.h"

#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\CustomVariantSettings\CustomVariantSettings.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "H2MOD\Modules\MapManager\MapManager.h"

#include "Util\Hooks\Hook.h"

BYTE g_network_message_type_collection[e_network_message_type_collection::_end * 32];

void register_network_message(void *network_message_collection, int type, char* name, int a4, int size1, int size2, void* write_packet_method, void* read_packet_method, void* unk_callback)
{
	typedef void(__thiscall* register_packet_type)(void *, int, char*, int, int, int, void*, void*, void*);
	auto register_packet = reinterpret_cast<register_packet_type>(Memory::GetAddress(0x1E81D6, 0x1CA199));
	return register_packet(network_message_collection, type, name, a4, size1, size2, write_packet_method, read_packet_method, unk_callback);
}

const char* GetNetworkMessageName(int enumVal) 
{
	return network_message_type_collection_name[enumVal];
}

bool MessageIsCustom(e_network_message_type_collection enumVal)
{
	return enumVal > e_network_message_type_collection::_test;
}

void __cdecl encode_map_file_name_message(bitstream* stream, int a2, s_custom_map_filename* data)
{
	stream->data_encode_string_wide("map-file-name", &data->file_name, ARRAYSIZE(data->file_name));
	stream->data_encode_integer("map-download-id", data->map_download_id, CHAR_BIT * sizeof(data->map_download_id));
}
bool __cdecl decode_map_file_name_message(bitstream* stream, int a2, s_custom_map_filename* data)
{
	stream->data_decode_string_wide("map-file-name", &data->file_name, ARRAYSIZE(data->file_name));
	data->map_download_id = stream->data_decode_integer("map-download-id", CHAR_BIT * sizeof(data->map_download_id));
	return stream->overflow() == false;
}

void __cdecl encode_request_map_filename_message(bitstream* stream, int a2, s_request_map_filename* data)
{
	stream->data_encode_bits("user-identifier", &data->user_identifier, player_identifier_size_bits);
	stream->data_encode_integer("map-download-id", data->map_download_id, CHAR_BIT * sizeof(data->map_download_id));
}
bool __cdecl decode_request_map_filename_message(bitstream* stream, int a2, s_request_map_filename* data)
{
	stream->data_decode_bits("user-identifier", &data->user_identifier, player_identifier_size_bits);
	data->map_download_id = stream->data_decode_integer("map-download-id", CHAR_BIT * sizeof(data->map_download_id));
	return stream->overflow() == false;
}

void __cdecl encode_team_change_message(bitstream* stream, int a2, s_team_change* data)
{
	stream->data_encode_integer("team-index", data->team_index, 32);
}
bool __cdecl decode_team_change_message(bitstream* stream, int a2, s_team_change* data)
{
	data->team_index = stream->data_decode_integer("team-index", 32);
	return stream->overflow() == false;
}

void __cdecl encode_rank_change_message(bitstream* stream, int a2, s_rank_change* data)
{
	stream->data_encode_integer("rank", data->rank, 8);
}

bool __cdecl decode_rank_change_message(bitstream* stream, int a2, s_rank_change* data)
{
	data->rank = stream->data_decode_integer("rank", 8);
	return stream->overflow() == false;
}

void __cdecl encode_anti_cheat_message(bitstream* stream, int a2, s_anti_cheat* data)
{
	stream->data_encode_bool("anti-cheat-enabled", data->enabled);
}
bool __cdecl decode_anti_cheat_message(bitstream* stream, int a2, s_anti_cheat* data)
{
	data->enabled = stream->data_decode_bool("anti-cheat-enabled");
	return stream->overflow() == false;
}

void register_custom_network_message(void* network_messages)
{
	typedef void(__cdecl* register_test_packet_t)(void* network_messages);
	auto p_register_test_message = Memory::GetAddress<register_test_packet_t>(0x1ECE05, 0x1CD7BE);

	p_register_test_message(network_messages);

	register_network_message(network_messages, _request_map_filename, "request-map-filename", 0, sizeof(s_request_map_filename), sizeof(s_request_map_filename),
		(void*)encode_request_map_filename_message, (void*)decode_request_map_filename_message, NULL);

	register_network_message(network_messages, _custom_map_filename, "map-file-name", 0, sizeof(s_custom_map_filename), sizeof(s_custom_map_filename),
		(void*)encode_map_file_name_message, (void*)decode_map_file_name_message, NULL);

	register_network_message(network_messages, _team_change, "team-change", 0, sizeof(s_team_change), sizeof(s_team_change),
		(void*)encode_team_change_message, (void*)decode_team_change_message, NULL);

	register_network_message(network_messages, _rank_change, "rank-change", 0, sizeof(s_rank_change), sizeof(s_rank_change),
		(void*)encode_rank_change_message, (void*)decode_rank_change_message, NULL);

	register_network_message(network_messages, _anti_cheat, "anti-cheat", 0, sizeof(s_anti_cheat), sizeof(s_anti_cheat),
		(void*)encode_anti_cheat_message, (void*)decode_anti_cheat_message, NULL);

	register_network_message(network_messages, _custom_variant_settings, "variant-settings", 0, CustomVariantSettingsPacketSize, CustomVariantSettingsPacketSize,
		(void*)CustomVariantSettings::EncodeVariantSettings, (void*)CustomVariantSettings::DecodeVariantSettings, NULL);
}

typedef void(__stdcall *handle_out_of_band_message_t)(void *thisx, network_address* address, e_network_message_type_collection message_type, int a4, void* packet);
handle_out_of_band_message_t p_handle_out_of_band_message;

void __stdcall handle_out_of_band_message_hook(void *thisx, network_address* address, e_network_message_type_collection message_type, int a4, void* packet)
{
	/* surprisingly the game doesn't use this too much, pretty much for request-join and tme-sync packets */
	LOG_TRACE_NETWORK("{} - Received message: {} from peer index: {}", 
		__FUNCTION__, GetNetworkMessageName(message_type), NetworkSession::GetPeerIndexFromNetworkAddress(address));

	if (!MessageIsCustom(message_type))
		p_handle_out_of_band_message(thisx, address, message_type, a4, packet);
}

typedef void(__stdcall *handle_channel_message_t)(void *thisx, int network_channel_index, e_network_message_type_collection message_type, int dynamic_data_size, void* packet);
handle_channel_message_t p_handle_channel_message;

void __stdcall handle_channel_message_hook(void *thisx, int network_channel_index, e_network_message_type_collection message_type, int dynamic_data_size, void* packet)
{
	/*
		This handles received in-band data
	*/

	network_address addr;
	ZeroMemory(&addr, sizeof(network_address));
	s_network_channel* peer_network_channel = s_network_channel::Get(network_channel_index);

	switch (message_type)
	{
	case _request_map_filename:
	{
		s_request_map_filename* received_data = (s_request_map_filename*)packet;
		LOG_TRACE_NETWORK("[H2MOD-CustomMessage] received on handle_channel_message_hook request-map-filename from XUID: {}", 
			received_data->user_identifier);
		if (peer_network_channel->channel_state == s_network_channel::e_channel_state::unk_state_5
			&& peer_network_channel->GetNetworkAddressFromNetworkChannel(&addr))
		{
			LOG_TRACE_NETWORK("  - network address: {:x}", ntohl(addr.address.ipv4));

			int peer_index = NetworkSession::GetPeerIndexFromNetworkAddress(&addr);
			s_network_session* session = NetworkSession::GetCurrentNetworkSession();
			if (peer_index != -1 && peer_index != session->local_peer_index)
			{
				s_custom_map_filename data;
				SecureZeroMemory(&data, sizeof(s_custom_map_filename));

				std::wstring map_filename;
				mapManager->GetMapFilename(map_filename);
				if (!map_filename.empty())
				{
					wcsncpy_s(data.file_name, map_filename.c_str(), map_filename.length());
					data.map_download_id = received_data->map_download_id;

					LOG_TRACE_NETWORK(L"[H2MOD-CustomMessage] sending map file name packet to XUID: {}, peer index: {}, map name: {}, download id {}", 
						received_data->user_identifier, 
						peer_index, map_filename.c_str(), received_data->map_download_id);

					s_network_observer* observer = session->p_network_observer;
					s_peer_observer_channel* observer_channel = NetworkSession::GetPeerObserverChannel(peer_index);

					observer->sendNetworkMessage(session->session_index, observer_channel->observer_index, s_network_observer::e_network_message_send_protocol::in_band, _custom_map_filename, sizeof(s_custom_map_filename), &data);
				}
				else
				{
					LOG_TRACE_NETWORK(L"[H2MOD-CustomMessage] no map file name found, abort sending packet! {} - {}", 
						received_data->user_identifier, peer_index, map_filename.c_str());
				}
			}
		}
		break;
	}

	case _custom_map_filename:
	{
		if (peer_network_channel->channel_state == s_network_channel::e_channel_state::unk_state_5)
		{
			s_custom_map_filename* received_data = (s_custom_map_filename*)packet;
			if (received_data->map_download_id != NONE)
			{
				auto map_download_query = mapManager->GetDownloadQueryById(received_data->map_download_id);
				if (map_download_query != nullptr)
				{
					map_download_query->SetMapNameToDownload(received_data->file_name);
					LOG_TRACE_NETWORK(L"[H2MOD-CustomMessage] received on handle_channel_message_hook custom_map_filename: {}",
						received_data->file_name);
				}
				else
				{
					// unlikely
					LOG_TRACE_NETWORK("[H2MOD-CustomMessage] - query with id {:X} hasn't been found!", 
						received_data->map_download_id);
					break;
				}
			}

			LOG_TRACE_NETWORK(L"[H2MOD-CustomMessage] - received map name: {}, no download ID", 
				received_data->file_name);
		}
		break;
	}

	case _team_change:
	{
		if (peer_network_channel->channel_state == s_network_channel::e_channel_state::unk_state_5)
		{
			s_team_change* received_data = (s_team_change*)packet;
			LOG_TRACE_NETWORK(L"[H2MOD-CustomMessage] recieved on handle_channel_message_hook team_change: {}", 
				received_data->team_index);
			h2mod->set_local_team_index(0, received_data->team_index);
		}
		break;
	}

	case _rank_change:
	{
		if (peer_network_channel->channel_state == s_network_channel::e_channel_state::unk_state_5)
		{
			s_rank_change* recieved_data = (s_rank_change*)packet;
			LOG_TRACE_NETWORK(L"H2MOD-CustomMessage] recieved on handle_channel_message_hook rank_change: {}", 
				recieved_data->rank);
			h2mod->set_local_rank(recieved_data->rank);
		}
		break;
	}

	case _anti_cheat:
	{
		if (peer_network_channel->channel_state == s_network_channel::e_channel_state::unk_state_5)
		{
			s_anti_cheat* recieved_data = (s_anti_cheat*)packet;
			H2Config_anti_cheat_enabled = recieved_data->enabled;
		}
		break;
	}
	
	case _custom_variant_settings:
	{
		if (peer_network_channel->channel_state == s_network_channel::e_channel_state::unk_state_5)
		{
			auto recieved_data = (CustomVariantSettings::s_variantSettings*)packet;
			CustomVariantSettings::UpdateCustomVariantSettings(recieved_data);
		}
		break;
	}

	// default packet
	case _leave_session:
	{
		if (peer_network_channel->channel_state == s_network_channel::e_channel_state::unk_state_5
			&& peer_network_channel->GetNetworkAddressFromNetworkChannel(&addr))
		{
			auto peer_index = NetworkSession::GetPeerIndexFromNetworkAddress(&addr);
			EventHandler::NetworkPlayerEventExecute(EventExecutionType::execute_before, peer_index, EventHandler::NetworkPlayerEventType::remove);
		}
		break; // don't return, leave the game to update state
	}

	default:
		break;
	} // switch (message_type)

	if (peer_network_channel->GetNetworkAddressFromNetworkChannel(&addr))
	{
		LOG_TRACE_NETWORK("{} - Received message: {} from peer index: {}, address: {:x}", 
			__FUNCTION__, GetNetworkMessageName(message_type), NetworkSession::GetPeerIndexFromNetworkAddress(&addr), ntohl(addr.address.ipv4));
	}
	else
	{
		LOG_ERROR_NETWORK("{} - Received message: {} from network channel: {} that maybe shouldn't have been received", 
			__FUNCTION__, GetNetworkMessageName(message_type), network_channel_index);
	}

	if (!MessageIsCustom(message_type))
		p_handle_channel_message(thisx, network_channel_index, message_type, dynamic_data_size, packet);

	if (message_type == _player_add) {
		if (peer_network_channel->channel_state == s_network_channel::e_channel_state::unk_state_5
			&& peer_network_channel->GetNetworkAddressFromNetworkChannel(&addr))
		{
			auto peer_index = NetworkSession::GetPeerIndexFromNetworkAddress(&addr);
			EventHandler::NetworkPlayerEventExecute(EventExecutionType::execute_after, peer_index, EventHandler::NetworkPlayerEventType::add);
			NetworkMessage::SendAntiCheat(peer_index);
		}
	}
}

void NetworkMessage::SendRequestMapFilename(int mapDownloadId)
{
	s_network_session* session = NetworkSession::GetCurrentNetworkSession();

	if (session->local_session_state == _network_session_state_peer_established)
	{
		s_request_map_filename data;
		XUserGetXUID(0, &data.user_identifier);
		data.map_download_id = mapDownloadId;

		s_network_observer* observer = session->p_network_observer;
		s_peer_observer_channel* observer_channel = NetworkSession::GetPeerObserverChannel(session->session_host_peer_index);

		if (observer_channel->field_1) {
			observer->sendNetworkMessage(session->session_index, observer_channel->observer_index, s_network_observer::e_network_message_send_protocol::in_band, _request_map_filename, sizeof(s_request_map_filename), &data);

			LOG_TRACE_NETWORK("{} session host peer index: {}, observer index {}, observer bool unk: {}, session index: {}",
				__FUNCTION__,
				session->session_host_peer_index,
				observer_channel->observer_index,
				observer_channel->field_1,
				session->session_index);
		}
	}
}

void NetworkMessage::SendTeamChange(int peerIdx, int teamIdx)
{
	s_network_session* session = NetworkSession::GetCurrentNetworkSession();
	if (NetworkSession::LocalPeerIsSessionHost())
	{
		s_team_change data;
		data.team_index = teamIdx;

		s_network_observer* observer = session->p_network_observer;
		s_peer_observer_channel* observer_channel = NetworkSession::GetPeerObserverChannel(peerIdx);

		if (peerIdx != -1 && !NetworkSession::PeerIndexLocal(peerIdx))
		{
			if (observer_channel->field_1) {
				observer->sendNetworkMessage(session->session_index, observer_channel->observer_index, s_network_observer::e_network_message_send_protocol::in_band, _team_change, sizeof(s_team_change), &data);
			}
		}
	}
}

void NetworkMessage::SendRankChange(int peerIdx, BYTE rank)
{
	s_network_session* session = NetworkSession::GetCurrentNetworkSession();
	if (NetworkSession::LocalPeerIsSessionHost())
	{
		s_rank_change data;
		data.rank = rank;

		s_network_observer* observer = session->p_network_observer;
		s_peer_observer_channel* observer_channel = NetworkSession::GetPeerObserverChannel(peerIdx);

		if (peerIdx != -1 && !NetworkSession::PeerIndexLocal(peerIdx))
		{
			if (observer_channel->field_1) {
				observer->sendNetworkMessage(session->session_index, observer_channel->observer_index, s_network_observer::e_network_message_send_protocol::in_band, _rank_change, sizeof(s_rank_change), &data);
			}
		}
	}
}
void NetworkMessage::SendAntiCheat(int peerIdx)
{
	s_network_session* session = NetworkSession::GetCurrentNetworkSession();

	if (NetworkSession::LocalPeerIsSessionHost())
	{
		s_network_observer* observer = session->p_network_observer;
		s_peer_observer_channel* observer_channel = NetworkSession::GetPeerObserverChannel(peerIdx);

		s_anti_cheat data;
		data.enabled = H2Config_anti_cheat_enabled;
		if (peerIdx != -1 && !NetworkSession::PeerIndexLocal(peerIdx)) {
			if (observer_channel->field_1) {
				observer->sendNetworkMessage(session->session_index, observer_channel->observer_index, s_network_observer::e_network_message_send_protocol::in_band, _anti_cheat, sizeof(s_anti_cheat), &data);
			}
		}
	}
}

void NetworkMessage::ApplyGamePatches()
{
	WritePointer(Memory::GetAddress(0x1AC733, 0x1AC901), g_network_message_type_collection);
	WritePointer(Memory::GetAddress(0x1AC8F8, 0x1ACAC6), g_network_message_type_collection);
	WriteValue<BYTE>(Memory::GetAddress(0x1E825E, 0x1CA221), e_network_message_type_collection::_end);
	WriteValue<int>(Memory::GetAddress(0x1E81C6, 0x1CA189), e_network_message_type_collection::_end * 32);

	PatchCall(Memory::GetAddress(0x1B5196, 0x1A8EF4), register_custom_network_message);

	p_handle_channel_message = (handle_channel_message_t)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1E929C, 0x1CB25C), (BYTE*)handle_channel_message_hook, 8);
	p_handle_out_of_band_message = (handle_out_of_band_message_t)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1E907B, 0x1CB03B), (BYTE*)handle_out_of_band_message_hook, 8);
}