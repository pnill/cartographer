#include "stdafx.h"
#include "CustomPackets.h"

#include "H2MOD.h"
#include "Util\Hooks\Hook.h"
#include "XLive\XUser\XUser.h"
#include "..\Memory\bitstream.h"
#include "..\..\MapManager\MapManager.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"

char g_network_message_types[e_network_message_types::end * 32];

void register_packet_impl(void *packetObject, int type, char* name, int a4, int size1, int size2, void* write_packet_method, void* read_packet_method, void* a9)
{
	typedef void(__thiscall* register_packet_type)(void *packetObject, int type, char* name, int a4, int size1, int size2, void* write_packet_method, void* read_packet_method, void* a9);
	auto register_packet = reinterpret_cast<register_packet_type>(h2mod->GetAddress(0x1E81D6, 0x1CA199));
	return register_packet(packetObject, type, name, a4, size1, size2, write_packet_method, read_packet_method, a9);
}

char* getNetworkChannelData(int channel_index)
{
	return *h2mod->GetAddress<char**>(0x4FADBC, 0x525274) + 248 * channel_index;
}

const char* getNetworkMessageName(int enumVal) 
{
	return network_message_name[enumVal];
}

bool getNetworkAddressFromNetworkChannel(char* network_channel, network_address* out_addr)
{
	typedef bool(__thiscall* get_network_address_from_network_channel)(char*, network_address*);
	auto p_get_network_address_from_network_channel = reinterpret_cast<get_network_address_from_network_channel>(h2mod->GetAddress(0x1BA543, 0x1C9364));
	return p_get_network_address_from_network_channel(network_channel, out_addr);
}

void __cdecl encode_map_file_name_packet(bitstream* stream, int a2, s_custom_map_filename* data)
{
	stream->data_encode_bool("is-custom-map", data->is_custom_map);
	if (data->is_custom_map == true)
		stream->data_encode_string("map-file-name", (int)&data->file_name, 32);
}
bool __cdecl decode_map_file_name_packet(bitstream* stream, int a2, s_custom_map_filename* data)
{
	data->is_custom_map = stream->data_decode_bool("is-custom-map");
	if (data->is_custom_map == true)
		stream->data_decode_string("map-file-name", (int)&data->file_name, 32);

	return stream->packet_is_valid() == false;
}

void __cdecl encode_request_map_filename_packet(bitstream* stream, int a2, s_request_map_filename* data)
{
	stream->data_encode_bits("user-identifier", &data->user_identifier, 64);
}
bool __cdecl decode_request_map_filename_packet(bitstream* stream, int a2, s_request_map_filename* data)
{
	stream->data_decode_bits("user-identifier", (int)&data->user_identifier, 64);
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

void __cdecl encode_set_grenades_packet(bitstream* stream, int a2, s_unit_grenades* data)
{
	stream->data_encode_integer("type", data->type, 32);
	stream->data_encode_integer("count", data->count, 32);
	stream->data_encode_integer("player-index", data->player_index, 32);
}
bool __cdecl decode_set_grenades_packet(bitstream* stream, int a2, s_unit_grenades* data)
{
	data->type = stream->data_decode_integer("type", 32);
	data->count = stream->data_decode_integer("count", 32);
	data->player_index = stream->data_decode_integer("player-index", 32);
	return stream->packet_is_valid() == false;
}

void register_custom_packets(void* network_messages)
{
	typedef void(__cdecl* register_test_packet)(void* network_messages);
	auto p_register_test_packet = h2mod->GetAddress<register_test_packet>(0x1ECE05, 0x1CD7BE);

	p_register_test_packet(network_messages);

	register_packet_impl(network_messages, request_map_filename, "request-map-filename", 0, sizeof(s_request_map_filename), sizeof(s_request_map_filename),
		(void*)encode_request_map_filename_packet, (void*)decode_request_map_filename_packet, NULL);

	register_packet_impl(network_messages, map_file_name, "map-file-name", 0, sizeof(s_custom_map_filename), sizeof(s_custom_map_filename),
		(void*)encode_map_file_name_packet, (void*)decode_map_file_name_packet, NULL);

	register_packet_impl(network_messages, team_change, "team-change", 0, sizeof(s_team_change), sizeof(s_team_change),
		(void*)encode_team_change_packet, (void*)decode_team_change_packet, NULL);

	register_packet_impl(network_messages, unit_grenades, "unit-grenades", 0, sizeof(s_unit_grenades), sizeof(s_unit_grenades),
		(void*)encode_set_grenades_packet, (void*)decode_set_grenades_packet, NULL);
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
				wcsncpy_s(data.file_name, map_filename.c_str(), 32);
			}

			LOG_TRACE_NETWORK(L"[H2MOD-CustomPackets] sending map file name packet to XUID: {}, peer index: {}, map name: {}", received_data->user_identifier, peer_index, map_filename.c_str());
			observer_channel_send_message(session->network_observer_ptr, session->unk_index, session->peer_observer_channels[peer_index].observer_index, true,
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
		if (!h2mod->Server)
			h2mod->set_local_player_unit_grenades(received_data->type, received_data->count, received_data->player_index);
		return;
	}

	default:
			break;
	}*/

	/* surprisingly the game doesn't use this too much, pretty much for request-join and tme-sync packets */
	LOG_TRACE_NETWORK("handle_out_of_band_message_hook() - Received message {} from peer index: {}", getNetworkMessageName(message_type), NetworkSession::getPeerIndexFromNetworkAddress(address));

	p_handle_out_of_band_message(thisx, address, message_type, a4, packet);
}

typedef void(__stdcall *handle_channel_message)(void *thisx, int network_channel_index, int message_type, int dynamic_data_size, void* packet);
handle_channel_message p_handle_channel_message;

void __stdcall handle_channel_message_hook(void *thisx, int network_channel_index, int message_type, int dynamic_data_size, void* packet)
{
	/*
		This handles received in-band data
	*/

	char* network_channel = getNetworkChannelData(network_channel_index);
	network_address addr;
	ZeroMemory(&addr, sizeof(network_address));

	switch (message_type)
	{
	case request_map_filename:
	{
		s_request_map_filename* received_data = (s_request_map_filename*)packet;
		LOG_TRACE_NETWORK("[H2MOD-CustomPackets] received on handle_channel_message_hook request-map-filename from XUID: {}", received_data->user_identifier);
		if (*(int*)(network_channel + 0x54) == 5 && getNetworkAddressFromNetworkChannel(network_channel, &addr))
		{
			int peer_index = NetworkSession::getPeerIndexFromNetworkAddress(&addr);
			network_session* session = NetworkSession::getCurrentNetworkSession();
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
					wcsncpy_s(data.file_name, map_filename.c_str(), 32);
				}

				LOG_TRACE_NETWORK(L"[H2MOD-CustomPackets] sending map file name packet to XUID: {}, peer index: {}, map name: {}", received_data->user_identifier, peer_index, map_filename.c_str());

				network_observer* observer = session->network_observer_ptr;
				peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(peer_index);

				observer->sendNetworkMessage(session->unk_index, observer_channel->observer_index, false, map_file_name, sizeof(s_custom_map_filename), &data);
			}
		}
		
		return;
	}
	case map_file_name:
	{
		if (*(int*)(network_channel + 0x54) == 5)
		{
			s_custom_map_filename* received_data = (s_custom_map_filename*)packet;
			if (received_data->is_custom_map)
			{
				std::wstring filename_wstr(received_data->file_name);
				std::string filename_str(filename_wstr.begin(), filename_wstr.end());
				mapManager->setMapFileNameToDownload(filename_str);
				LOG_TRACE_NETWORK(L"[H2MOD-CustomPackets] received on handle_out_of_band_message map_file_name: {}", received_data->file_name);
			}
		}
		
		return;
	}

	case team_change:
	{
		if (*(int*)(network_channel + 0x54) == 5)
		{
			s_team_change* received_data = (s_team_change*)packet;
			h2mod->set_local_team_index(0, received_data->team_index);
			return;
		}
	}

	case unit_grenades:
	{
		if (*(int*)(network_channel + 0x54) == 5)
		{
			s_unit_grenades* received_data = (s_unit_grenades*)packet;
			if (!h2mod->Server)
				h2mod->set_player_unit_grenades_count(received_data->type, received_data->count, received_data->player_index);
		}
		return;
	}


	default:
		break;
	}

	p_handle_channel_message(thisx, network_channel_index, message_type, dynamic_data_size, packet);
}

void CustomPackets::sendRequestMapFilename()
{
	network_session* session = NetworkSession::getCurrentNetworkSession();

	if (session->local_session_state == network_session_state_peer_established)
	{
		XUID xuid;
		s_request_map_filename data;

		XUserGetXUID(0, &xuid);

		SecureZeroMemory(&data, sizeof(s_request_map_filename));
		data.user_identifier = xuid;

		network_observer* observer = session->network_observer_ptr;
		peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(session->session_host_peer_index);

		LOG_TRACE_NETWORK("[H2MOD-CustomPackets] Sending map name request info: session host peer index: {}, observer index {}, observer bool unk: {}, unk index: {}",
			session->session_host_peer_index, 
			observer_channel->observer_index,
			observer_channel->field_1,
			session->unk_index);

		// use this check for out-of-band packets
		//if (observer->getObserverState(observer_channel->observer_index) == 7) {
		//}
		if (observer_channel->field_1) {
			observer->sendNetworkMessage(session->unk_index, observer_channel->observer_index, false, request_map_filename, sizeof(s_request_map_filename), &data);
		}
	}
}

void CustomPackets::sendTeamChange(int peerIndex, int teamIndex)
{
	network_session* session = NetworkSession::getCurrentNetworkSession();
	if (session->local_session_state == network_session_state_session_host)
	{
		s_team_change data;
		data.team_index = teamIndex;

		network_observer* observer = session->network_observer_ptr;
		peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(peerIndex);

		if (peerIndex != -1 && peerIndex != session->local_peer_index)
		{
			//if (observer->getObserverState(observer_channel->observer_index) == 7) {
				if (observer_channel->field_1) {
					observer->sendNetworkMessage(session->unk_index, observer_channel->observer_index, false, team_change, sizeof(s_team_change), &data);
				}
			//}
		}
	}
}

void CustomPackets::sendUnitGrenadesPacket(int peerIndex, s_unit_grenades* data)
{
	network_session* session = NetworkSession::getCurrentNetworkSession();
	if (session->local_session_state == network_session_state_session_host)
	{
		if (peerIndex != -1 && peerIndex != session->local_peer_index)
		{
			network_observer* observer = session->network_observer_ptr;
			peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(peerIndex);

			//if (observer->getObserverState(observer_channel->observer_index) == 7) {
				if (observer_channel->field_1) {
					observer->sendNetworkMessage(session->unk_index, observer_channel->observer_index, false, unit_grenades, sizeof(s_unit_grenades), data);
				}
			//}
		}
	}
}

void CustomPackets::ApplyGamePatches()
{
	WritePointer(h2mod->GetAddress(0x1AC733, 0x1AC901), g_network_message_types);
	WritePointer(h2mod->GetAddress(0x1AC8F8, 0x1ACAC6), g_network_message_types);
	WriteValue<BYTE>(h2mod->GetAddress(0x1E825E, 0x1CA221), e_network_message_types::end);
	WriteValue<int>(h2mod->GetAddress(0x1E81C6, 0x1CA189), e_network_message_types::end * 32);

	PatchCall(h2mod->GetAddress(0x1B5196, 0x1A8EF4), register_custom_packets);

	p_handle_out_of_band_message = (handle_out_of_band_message)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1E907B, 0x1CB03B), (BYTE*)handle_out_of_band_message_hook, 8);
	p_handle_channel_message = (handle_channel_message)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1E929C, 0x1CB25C), (BYTE*)handle_channel_message_hook, 8);
}