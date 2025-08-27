#include "stdafx.h"
#include "network_message_handler.h"

#include "network_messages_cartographer.h"

#include "cartographer/twizzler/twizzler.h"
#include "networking/delivery/network_channel.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/messages/network_message_gateway.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_manager.h"
#include "networking/session/network_observer.h"
#include "networking/transport/transport_security.h"
#include "networking/network_event.h"

#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "H2MOD/Modules/MapManager/MapManager.h"

/* constants */


/* typedefs */

typedef void(__stdcall* t_read_channel_message)(c_network_message_handler* thisx, int32 network_channel_index, e_network_message_type message_type, int32 message_storage_size, uint8* packet);

typedef void(__stdcall* t_handle_out_of_band_message)(c_network_message_handler* thisx, transport_address* address, e_network_message_type message_type, int32 a4, uint8* packet);

/* prototypes */

static void __stdcall handle_out_of_band_message_hook(c_network_message_handler* thisx, transport_address* address, e_network_message_type message_type, int32 a4, uint8* packet);
static void __stdcall read_channel_message_hook(c_network_message_handler* thisx, int32 network_channel_index, e_network_message_type message_type, int32 message_storage_size, uint8* packet);

/* globals */

t_read_channel_message p_read_channel_message;
t_handle_out_of_band_message p_handle_out_of_band_message;

/* public code */

void network_message_handler_apply_patches(void)
{
	p_read_channel_message = (t_read_channel_message)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1E929C, 0x1CB25C), (BYTE*)read_channel_message_hook, 8);
	p_handle_out_of_band_message = (t_handle_out_of_band_message)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1E907B, 0x1CB03B), (BYTE*)handle_out_of_band_message_hook, 8);
}

/* private code */

void __stdcall handle_out_of_band_message_hook(c_network_message_handler* thisx, transport_address* address, e_network_message_type message_type, int32 a4, uint8* packet)
{
	c_network_session* session;
	if (network_life_cycle_in_squad_session(&session))
	{
		/* surprisingly the game doesn't use this too much, pretty much for request-join and time-sync packets */
		event(_event_message, "h2mod:custom_message: %s - Received message: %s from peer index: %d",
			__FUNCTION__, get_network_message_description(message_type), session->get_peer_index_from_address(address));
	}

	if (!is_message_custom(message_type))
		p_handle_out_of_band_message(thisx, address, message_type, a4, packet);
}

void __stdcall read_channel_message_hook(c_network_message_handler* thisx, int32 network_channel_index, e_network_message_type message_type, int32 message_storage_size, uint8* packet)
{
	/*
		This handles received in-band data
	*/

	transport_address addr{};
	s_network_channel* peer_network_channel = s_network_channel::get(network_channel_index);

	switch (message_type)
	{
	case _network_message_type_request_map_filename:
	{
		if (peer_network_channel->is_channel_state_5()
			&& peer_network_channel->get_network_address(&addr))
		{
			thisx->handle_request_map_filename(&addr, (s_network_message_request_map_filename*)packet);
		}
		break;
	}

	case _network_message_type_custom_map_filename:
	{
		if (peer_network_channel->is_channel_state_5()
			&& peer_network_channel->get_network_address(&addr))
		{
			thisx->handle_map_filename_response(&addr, network_channel_index, (s_network_message_custom_map_filename*)packet);
		}
		break;
	}

	case _network_message_type_rank_change:
	{
		if (peer_network_channel->is_channel_state_5()
			&& peer_network_channel->get_network_address(&addr))
		{
			thisx->handle_player_property_rank(&addr, network_channel_index, (s_network_message_rank_change*)packet);
		}
		break;
	}

	case _network_message_type_anti_cheat:
	{
		if (peer_network_channel->is_channel_state_5()
			&& peer_network_channel->get_network_address(&addr))
		{
			thisx->handle_session_anticheat_status(&addr, network_channel_index, (s_network_message_anti_cheat*)packet);
		}
		break;
	}

	// default packet
	case _network_message_type_leave_session:
	{
		if (peer_network_channel->is_channel_state_5()
			&& peer_network_channel->get_network_address(&addr))
		{
			thisx->handle_leave_session(&addr, (s_network_message_session_data*)packet);
		}
		break; // don't return, leave the game to update state
	}

	default:
		break;
	} // switch (message_type)

	if (peer_network_channel->get_network_address(&addr))
	{
		event(_event_verbose, "h2mod:custom_message: %s - Received message: %s from network channel: %d, address: %x",
			__FUNCTION__, get_network_message_description(message_type), network_channel_index, ntohl(addr.address.raw_ipv4));
	}
	else
	{
		event(_event_error, "h2mod:custom_message: %s - Received message: %s from an unestablished network channel: %d",
			__FUNCTION__, get_network_message_description(message_type), network_channel_index);
	}

	if (!is_message_custom(message_type))
		p_read_channel_message(thisx, network_channel_index, message_type, message_storage_size, packet);

	switch (message_type)
	{
	case _network_message_type_membership_update:
		if (peer_network_channel->is_channel_state_5()
			&& peer_network_channel->get_network_address(&addr))
		{
			thisx->handle_membership_update(&addr, network_channel_index, (s_network_message_session_data*)packet);
		}
		break;
	case _network_message_type_player_add:
		if (peer_network_channel->is_channel_state_5()
			&& peer_network_channel->get_network_address(&addr))
		{
			thisx->handle_player_add(&addr, (s_network_message_session_data*)packet);
		}
		break;
	default:
		break;
	}
}

bool c_network_message_handler::initialize_handler(c_network_link* link, const c_network_message_type_collection* message_types, c_network_message_gateway* message_gateway)
{
	ASSERT(!m_initialized);
	ASSERT(link);
	ASSERT(message_types);
	ASSERT(message_gateway);

	m_link = link;
	m_message_types = message_types;
	m_message_gateway = message_gateway;
	m_message_gateway->attach_handler(this);
	m_observer = NULL;
	m_session_manager = NULL;
	m_initialized = true;
	return true;
}

void c_network_message_handler::register_session_manager(c_network_session_manager* session_manager)
{
	ASSERT(m_initialized);
	ASSERT(session_manager);
	ASSERT(m_session_manager == NULL);
	m_session_manager = session_manager;
	return;
}

void c_network_message_handler::register_observer(c_network_observer* observer)
{
	ASSERT(m_initialized);
	ASSERT(observer);
	ASSERT(m_observer == NULL);
	m_observer = observer;
	return;
}

/* ### TODO move these handlers to separate files */
/* ### TODO move this to cartographer session */

void c_network_message_handler::handle_request_map_filename(const transport_address* address, const s_network_message_request_map_filename* received_data)
{
	c_network_session* session = m_session_manager->get_session(&received_data->session_data.identifier);
	if (session)
	{
		s_network_message_custom_map_filename data;
		event(_event_message, "h2mod:custom_message: received on read_channel_message_hook request-map-filename from XUID: %llu", received_data->player_id);

		int32 sender_peer_index = session->get_peer_index_from_address(address);
		if (sender_peer_index != NONE
			&& session->is_host()
			&& !session->is_peer_local(sender_peer_index))
		{
			std::wstring map_filename;
			if (mapManager->GetMapFilename(map_filename))
			{
				session->get_transport_session_id(&data.session_data.identifier);
				wcsncpy_s(data.file_name, map_filename.c_str(), map_filename.length());
				data.map_download_id = received_data->map_download_id;

				event(
					_event_message,
					"h2mod:custom_message: sending map file name packet to player id: %llu, peer index: %d, map name: %ws, download id %d",
					received_data->player_id,
					sender_peer_index,
					map_filename.c_str(),
					received_data->map_download_id
				);

				c_network_observer* observer = session->m_network_observer;
				s_session_peer* peer = session->get_session_peer(sender_peer_index);

				if (peer->is_remote_peer)
					observer->send_message(session->m_session_index, peer->observer_channel_index, false, _network_message_type_custom_map_filename, sizeof(s_network_message_custom_map_filename), &data);
			}
			else
			{
				event(
					_event_message,
					"h2mod:custom_message: no map file name found, aborting response for player id: %llu, peer idx: %d",
					received_data->player_id,
					sender_peer_index
				);
			}
		}
	}
	return;
}

void c_network_message_handler::handle_map_filename_response(const transport_address* address, int32 channel_index, const s_network_message_custom_map_filename* received_data)
{
	c_network_session* session = m_session_manager->get_session(&received_data->session_data.identifier);
	if (session)
	{
		if (session->channel_is_authoritative(channel_index))
		{
			if (received_data->map_download_id != NONE)
			{
				auto map_download_query = mapManager->GetDownloadQueryById(received_data->map_download_id);
				if (map_download_query != nullptr)
				{
					map_download_query->SetMapNameToDownload(received_data->file_name);
					event(_event_message, "h2mod:custom_message: received on read_channel_message_hook custom_map_filename: %ws", received_data->file_name);
				}
				else
				{
					event(_event_message, "h2mod:custom_message: query with id %X hasn't been found!", received_data->map_download_id);
				}
			}
		}
	}
}

void c_network_message_handler::handle_player_property_rank(const transport_address* address, int32 channel_index, const s_network_message_rank_change* received_data)
{
	c_network_session* session = m_session_manager->get_session(&received_data->session_data.identifier);
	if (session)
	{
		if (session->channel_is_authoritative(channel_index))
		{
			event(_event_message, "h2mod:custom_message: recieved on read_channel_message_hook rank_change: %d", received_data->rank);
			network_session_interface_set_local_user_rank(0, received_data->rank);
		}
	}
}

void c_network_message_handler::handle_session_anticheat_status(const transport_address* address, int32 channel_index, const s_network_message_anti_cheat* received_data)
{
	c_network_session* session = m_session_manager->get_session(&received_data->session_data.identifier);
	if (session)
	{
		if (session->channel_is_authoritative(channel_index))
		{
			twizzler_set_status(received_data->enabled);
		}
	}
}

void c_network_message_handler::handle_leave_session(const transport_address* address, const s_network_message_session_data* received_data)
{
	c_network_session* session = m_session_manager->get_session(&received_data->identifier);
	if (session)
	{
		int32 sender_peer_index = session->get_peer_index_from_address(address);

		if (sender_peer_index != NONE
			&& !session->is_peer_local(sender_peer_index))
		{
			EventHandler::NetworkPlayerEventExecute(EventExecutionType::execute_before, sender_peer_index, EventHandler::NetworkPlayerEventType::remove);
		}
	}
}

void c_network_message_handler::handle_membership_update(const transport_address* address, int32 channel_index, const s_network_message_session_data* received_data)
{
	c_network_session* session = m_session_manager->get_session(&received_data->identifier);
	if (session)
	{
		if (session->channel_is_authoritative(channel_index))
		{
			network_session_membership_update_local_players_teams();
		}
	}
}

void c_network_message_handler::handle_player_add(const transport_address* address, const s_network_message_session_data* received_data)
{
	c_network_session* session = m_session_manager->get_session(&received_data->identifier);
	if (session)
	{
		int32 sender_peer_index = session->get_peer_index_from_address(address);

		if (sender_peer_index != NONE
			&& !session->is_peer_local(sender_peer_index))
		{
			EventHandler::NetworkPlayerEventExecute(EventExecutionType::execute_after, sender_peer_index, EventHandler::NetworkPlayerEventType::add);

			if (session->is_host())
			{
				network_message_cartographer_send_anti_cheat(sender_peer_index);
			}
		}
	}
}