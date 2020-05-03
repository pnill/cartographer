#include "stdafx.h"
#include "CustomPackets.h"

#include "H2MOD.h"
#include "Util\Hooks\Hook.h"
#include "..\Memory\bitstream.h"
#include "..\..\MapManager\MapManager.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"

extern XUID xFakeXuid[4];
char g_network_message_types[e_network_message_types::end * 32]; // out of band packets

void register_packet_impl(void *packetObject, int type, char* name, int a4, int size1, int size2, void* write_packet_method, void* read_packet_method, void* a9)
{
	typedef void(__thiscall* register_packet_type)(void *packetObject, int type, char* name, int a4, int size1, int size2, void* write_packet_method, void* read_packet_method, void* a9);
	auto register_packet = reinterpret_cast<register_packet_type>(h2mod->GetAddress(0x1E81D6, 0x1CA199));
	return register_packet(packetObject, type, name, a4, size1, size2, write_packet_method, read_packet_method, a9);
}

void observer_channel_send_message(void *observer, int unk_index, int observer_index, bool send_out_of_band, int type, size_t size, void* buffer)
{
	typedef void(__thiscall* observer_channel_send_message)(void *observer, int unk_index, int observer_index, bool send_out_of_band, int type, size_t size, void* buffer);
	auto p_dynamic_packet_check = reinterpret_cast<observer_channel_send_message>(h2mod->GetAddress(0x1BED40, 0x1B8C1A));
	p_dynamic_packet_check(observer, unk_index, observer_index, send_out_of_band, type, size, buffer);
}

void __cdecl encode_map_file_name_packet(char* buffer, int a2, s_custom_map_filename* data)
{
	bitstream::p_data_encode_bool()(buffer, "is-custom-map", data->is_custom_map);
	bitstream::p_data_encode_string()(buffer, "map-file-name", (int)&data->file_name, 32);
}
bool __cdecl decode_map_file_name_packet(char* buffer, int a2, s_custom_map_filename* data)
{
	data->is_custom_map = bitstream::p_data_decode_bool()(buffer, "is-custom-map");
	bitstream::p_data_decode_string()(buffer, "map-file-name", (int)&data->file_name, 32);
	return bitstream::p_packet_is_valid()(buffer) == 0;
}

void __cdecl encode_request_map_filename_packet(char* buffer, int a2, s_request_map_filename* data)
{
	bitstream::p_data_encode_bits()(buffer, "user-identifier", &data->user_identifier, 64);
}
bool __cdecl decode_request_map_filename_packet(char* buffer, int a2, s_request_map_filename* data)
{
	bitstream::p_data_decode_bits()(buffer, "user-identifier", (int)&data->user_identifier, 64);
	return bitstream::p_packet_is_valid()(buffer) == 0;
}

void __cdecl encode_team_change_packet(char* buffer, int a2, s_team_change* data)
{
	bitstream::p_data_encode_integer()(buffer, "team-index", data->team_index, 32);
}
bool __cdecl decode_team_change_packet(char* buffer, int a2, s_team_change* data)
{
	data->team_index = bitstream::p_data_decode_integer()(buffer, "team-index", 32);
	return bitstream::p_packet_is_valid()(buffer) == 0;
}

void __cdecl encode_set_grenades_packet(char* buffer, int a2, s_unit_grenades* data)
{
	bitstream::p_data_encode_integer()(buffer, "type", data->type, 32);
	bitstream::p_data_encode_integer()(buffer, "count", data->count, 32);
	bitstream::p_data_encode_integer()(buffer, "player-index", data->player_index, 32);
}
bool __cdecl decode_set_grenades_packet(char* buffer, int a2, s_unit_grenades* data)
{
	data->type = bitstream::p_data_decode_integer()(buffer, "type", 32);
	data->count = bitstream::p_data_decode_integer()(buffer, "count", 32);
	data->player_index = bitstream::p_data_decode_integer()(buffer, "player-index", 32);
	return bitstream::p_packet_is_valid()(buffer) == 0;
}

void register_custom_packets(void* a1)
{
	typedef void(__cdecl* register_test_packet)(void* a1);
	auto p_register_test_packet = h2mod->GetAddress<register_test_packet>(0x1ECE05, 0x1CD7BE);
	p_register_test_packet(a1);

	register_packet_impl(g_network_message_types, map_file_name, "map-file-name", 0, sizeof(s_custom_map_filename), sizeof(s_custom_map_filename), 
		(void*)encode_map_file_name_packet, (void*)decode_map_file_name_packet, NULL);

	register_packet_impl(g_network_message_types, request_map_filename, "request-map-filename", 0, sizeof(s_request_map_filename), sizeof(s_request_map_filename),
		(void*)encode_request_map_filename_packet, (void*)decode_request_map_filename_packet, NULL);

	register_packet_impl(g_network_message_types, team_change, "team-change", 0, sizeof(s_team_change), sizeof(s_team_change),
		(void*)encode_team_change_packet, (void*)decode_team_change_packet, NULL);

	register_packet_impl(g_network_message_types, unit_grenades, "unit-grenades", 0, sizeof(s_unit_grenades), sizeof(s_unit_grenades),
		(void*)encode_set_grenades_packet, (void*)decode_set_grenades_packet, NULL);
}

typedef void(__stdcall *handle_out_of_band_message)(void *thisx, network_address* address, int message_type, int a4, void* packet);
handle_out_of_band_message p_handle_out_of_band_message;

void __stdcall handle_out_of_band_message_hook(void *thisx, network_address* address, int message_type, int a4, void* packet)
{
	/*
		This handles received out-of-band data
	*/

	switch (message_type)
	{
	case request_map_filename:
	{
		s_request_map_filename* received_data = (s_request_map_filename*)packet;
		signed int peer_index = NetworkSession::getPeerIndexFromNetworkAddress(address);
		LOG_TRACE_NETWORK("[H2MOD-CustomPackets] received on handle_out_of_band_message request-map-filename from XUID: {}, peer index: {}", received_data->user_identifier, peer_index);
		network_session* current_session = NetworkSession::getCurrentNetworkSession();
		if (peer_index != -1 && peer_index != current_session->local_peer_index)
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

			observer_channel_send_message(current_session->network_observer, current_session->unk_index, current_session->peer_observer_channels[peer_index].observer_index, true,
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
			h2mod->set_local_grenades(received_data->type, received_data->count, received_data->player_index);
		return;
	}

	default:
			break;
	}

	p_handle_out_of_band_message(thisx, address, message_type, a4, packet);
}

typedef void(__stdcall *handle_channel_message)(void *thisx, int network_channel_index, int message_type, int dynamic_data_size, void* packet);
handle_channel_message p_handle_channel_message;

void __stdcall handle_channel_message_hook(void *thisx, int network_channel_index, int message_type, int dynamic_data_size, void* packet)
{
	/*
		This handles received in-band data
	*/

	switch (message_type)
	{
	case request_map_filename:
	{
		s_request_map_filename* received_packet = (s_request_map_filename*)packet;
		LOG_TRACE_NETWORK("[H2MOD-CustomPackets] received on handle_channel_message_hook request-map-filename from XUID: {:#x}", received_packet->user_identifier);
		return;
	}
	case map_file_name:
		return;

	default:
		break;
	}

	p_handle_channel_message(thisx, network_channel_index, message_type, dynamic_data_size, packet);
}

void CustomPackets::sendRequestMapFilename(network_session* session)
{
	if (session->local_session_state == network_session_state_peer_established)
	{
		s_request_map_filename data;
		SecureZeroMemory(&data, sizeof(s_request_map_filename));
		memcpy(&data.user_identifier, &xFakeXuid[0], sizeof(XUID));

		LOG_TRACE_NETWORK("[H2MOD-CustomPackets] Sending map name request info: session host peer index: {}, observer index {}, observer bool unk: {}",
			session->session_host_peer_index, 
			session->peer_observer_channels[session->session_host_peer_index].observer_index,
			session->peer_observer_channels[session->session_host_peer_index].field_1);

		if (session->peer_observer_channels[session->session_host_peer_index].field_1) {
			observer_channel_send_message(session->network_observer, session->unk_index, session->peer_observer_channels[session->session_host_peer_index].observer_index, true,
				request_map_filename, sizeof(s_request_map_filename), (void*)&data);
		}
	}
}

void CustomPackets::sendTeamChange(network_session* session, signed int peer_index, int team_index)
{
	if (session->local_session_state == network_session_state_session_host)
	{
		s_team_change buffer;
		buffer.team_index = team_index;

		if (peer_index != -1 && peer_index != session->local_peer_index)
		{
			if (session->peer_observer_channels[peer_index].field_1)
			{
				observer_channel_send_message(session->network_observer, session->unk_index, session->peer_observer_channels[peer_index].observer_index, true,
					team_change, sizeof(s_team_change), (void*)&buffer);
			}
		}
	}
}

void CustomPackets::sendUnitGrenadesPacket(network_session* session, int peer_index, s_unit_grenades* data)
{
	if (session->local_session_state == network_session_state_session_host)
	{
		if (peer_index != -1 && peer_index != session->local_peer_index)
		{
			if (session->peer_observer_channels[peer_index].field_1)
			{
				observer_channel_send_message(session->network_observer, session->unk_index, session->peer_observer_channels[peer_index].observer_index, true,
					unit_grenades, sizeof(s_unit_grenades), (void*)data);
			}
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

	DWORD dwBack;
	p_handle_out_of_band_message = (handle_out_of_band_message)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1E907B, 0x1CB03B), (BYTE*)handle_out_of_band_message_hook, 8);

	p_handle_channel_message = (handle_channel_message)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1E929C, 0x1CB25C), (BYTE*)handle_channel_message_hook, 8);
}