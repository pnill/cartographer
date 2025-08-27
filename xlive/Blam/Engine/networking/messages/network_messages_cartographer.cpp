#include "stdafx.h"
#include "network_messages_cartographer.h"

#include "network_message_type_collection.h"

#include "cartographer/twizzler/twizzler.h"
#include "memory/bitstream.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/session/network_observer.h"
#include "networking/session/network_session.h"
#include "networking/network_event.h"

/* prototypes */

static void encode_map_file_name_message(c_bitstream* stream, int a2, const s_network_message_custom_map_filename* data);

static bool decode_map_file_name_message(c_bitstream* stream, int a2, s_network_message_custom_map_filename* data);

static void encode_request_map_filename_message(c_bitstream* stream, int a2, const s_network_message_request_map_filename* data);

static bool decode_request_map_filename_message(c_bitstream* stream, int a2, s_network_message_request_map_filename* data);

static void encode_rank_change_message(c_bitstream* stream, int a2, const s_network_message_rank_change* data);

static bool decode_rank_change_message(c_bitstream* stream, int a2, s_network_message_rank_change* data);

static void encode_anti_cheat_message(c_bitstream* stream, int a2, const s_network_message_anti_cheat* data);

static bool decode_anti_cheat_message(c_bitstream* stream, int a2, s_network_message_anti_cheat* data);

/* public code */

void network_message_types_register_cartographer_types(c_network_message_type_collection* message_collection)
{
	message_collection->register_message_type(
		_network_message_type_request_map_filename,
		"request-map-filename",
		0,
		sizeof(s_network_message_request_map_filename),
		sizeof(s_network_message_request_map_filename),
		encode_request_map_filename_message,
		decode_request_map_filename_message,
		NULL);

	message_collection->register_message_type(
		_network_message_type_custom_map_filename,
		"map-file-name",
		0,
		sizeof(s_network_message_custom_map_filename),
		sizeof(s_network_message_custom_map_filename),
		encode_map_file_name_message,
		decode_map_file_name_message,
		NULL);

	message_collection->register_message_type(
		_network_message_type_rank_change,
		"rank-change",
		0,
		sizeof(s_network_message_rank_change),
		sizeof(s_network_message_rank_change),
		encode_rank_change_message,
		decode_rank_change_message,
		NULL);

	message_collection->register_message_type(
		_network_message_type_anti_cheat,
		"",
		0,
		sizeof(s_network_message_anti_cheat),
		sizeof(s_network_message_anti_cheat),
		encode_anti_cheat_message,
		decode_anti_cheat_message,
		NULL);

	return;
}

void network_message_cartographer_send_request_map_filename(int32 map_download_id)
{
	c_network_session* session = NULL;
	s_network_message_request_map_filename data;

	if (network_life_cycle_in_squad_session(&session)
		&& session->established()
		&& !session->is_host()
		&& session->get_transport_session_id(&data.session_data.identifier))
	{
		XUserGetXUID(0, &data.player_id);
		data.map_download_id = map_download_id;

		c_network_observer* observer = session->m_network_observer;
		s_session_peer* peer = session->get_session_peer(session->m_session_host_peer_index);

		if (peer->is_remote_peer)
		{
			observer->send_message(session->m_session_index, peer->observer_channel_index, false, _network_message_type_request_map_filename, sizeof(s_network_message_request_map_filename), &data);

			event(_event_status, "online:messages: %s session host peer index: %d, observer index %d, observer is remote peer: %d, session index: %d",
				__FUNCTION__,
				session->m_session_host_peer_index,
				peer->observer_channel_index,
				peer->is_remote_peer,
				session->m_session_index);
		}
	}
	return;
}

void network_message_cartographer_send_rank_change(int32 peer_index, int8 rank)
{
	c_network_session* session = NULL;
	s_network_message_rank_change data;

	if (network_life_cycle_in_squad_session(&session)
		&& session->is_host()
		&& session->get_transport_session_id(&data.session_data.identifier))
	{
		data.rank = rank;

		c_network_observer* observer = session->m_network_observer;
		s_session_peer* peer = session->get_session_peer(peer_index);

		if (peer_index != NONE && !session->is_peer_local(peer_index))
		{
			if (peer->is_remote_peer) {
				observer->send_message(session->m_session_index, peer->observer_channel_index, false, _network_message_type_rank_change, sizeof(s_network_message_rank_change), &data);
			}
		}
	}
	return;
}

void network_message_cartographer_send_anti_cheat(int32 peer_index)
{
	c_network_session* session = NULL;
	s_network_message_anti_cheat data;

	if (network_life_cycle_in_squad_session(&session)
		&& session->is_host()
		&& session->get_transport_session_id(&data.session_data.identifier))
	{
		c_network_observer* observer = session->m_network_observer;
		s_session_peer* peer = session->get_session_peer(peer_index);

		data.enabled = g_twizzler_status;
		if (peer_index != NONE && !session->is_peer_local(peer_index)) {
			if (peer->is_remote_peer) {
				observer->send_message(session->m_session_index, peer->observer_channel_index, false, _network_message_type_anti_cheat, sizeof(s_network_message_anti_cheat), &data);
			}
		}
	}
	return;
}

/* private code */

static void encode_map_file_name_message(c_bitstream* stream, int a2, const s_network_message_custom_map_filename* data)
{
	stream->write_raw_data("session-id", &data->session_data.identifier, SIZEOF_BITS(data->session_data.identifier));
	stream->write_string_wchar("map-file-name", data->file_name, ARRAYSIZE(data->file_name));
	stream->write_integer("map-download-id", data->map_download_id, SIZEOF_BITS(data->map_download_id));
	return;
}

static bool decode_map_file_name_message(c_bitstream* stream, int a2, s_network_message_custom_map_filename* data)
{
	stream->read_raw_data("session-id", &data->session_data.identifier, SIZEOF_BITS(data->session_data.identifier));
	stream->read_string_wchar("map-file-name", data->file_name, ARRAYSIZE(data->file_name));
	data->map_download_id = stream->read_integer("map-download-id", SIZEOF_BITS(data->map_download_id));
	return stream->error_occured() == false;
}

static void encode_request_map_filename_message(c_bitstream* stream, int a2, const s_network_message_request_map_filename* data)
{
	stream->write_raw_data("session-id", &data->session_data.identifier, SIZEOF_BITS(data->session_data.identifier));
	stream->write_raw_data("user-identifier", &data->player_id, SIZEOF_BITS(data->player_id));
	stream->write_integer("map-download-id", data->map_download_id, SIZEOF_BITS(data->map_download_id));
	return;
}

static bool decode_request_map_filename_message(c_bitstream* stream, int a2, s_network_message_request_map_filename* data)
{
	stream->read_raw_data("session-id", &data->session_data.identifier, SIZEOF_BITS(data->session_data.identifier));
	stream->read_raw_data("user-identifier", &data->player_id, SIZEOF_BITS(data->player_id));
	data->map_download_id = stream->read_integer("map-download-id", SIZEOF_BITS(data->map_download_id));
	return stream->error_occured() == false;
}

static void encode_rank_change_message(c_bitstream* stream, int a2, const s_network_message_rank_change* data)
{
	stream->write_raw_data("session-id", &data->session_data.identifier, SIZEOF_BITS(data->session_data.identifier));
	stream->write_integer("rank", data->rank, SIZEOF_BITS(data->rank));
	return;
}
static bool decode_rank_change_message(c_bitstream* stream, int a2, s_network_message_rank_change* data)
{
	stream->read_raw_data("session-id", &data->session_data.identifier, SIZEOF_BITS(data->session_data.identifier));
	data->rank = (int8)stream->read_integer("rank", SIZEOF_BITS(data->rank));
	return stream->error_occured() == false;
}

static void encode_anti_cheat_message(c_bitstream* stream, int a2, const s_network_message_anti_cheat* data)
{
	stream->write_raw_data("session-id", &data->session_data.identifier, SIZEOF_BITS(data->session_data.identifier));
	stream->write_bool("", data->enabled);
	return;
}
static bool decode_anti_cheat_message(c_bitstream* stream, int a2, s_network_message_anti_cheat* data)
{
	stream->read_raw_data("session-id", &data->session_data.identifier, SIZEOF_BITS(data->session_data.identifier));
	data->enabled = stream->read_bool("");
	return stream->error_occured() == false;
}
