#include "stdafx.h"
#include "network_globals.h"

#include "network_configuration.h"
#include "network_event.h"
#include "network_memory.h"
#include "network_time.h"

#include "game/game.h"
#include "networking/delivery/network_link.h"
#include "networking/locator/network_locator.h"
#include "networking/logic/network_bandwidth.h"
#include "networking/logic/network_broadcast_search.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/logic/network_search.h"
#include "networking/logic/network_session_interface.h"
#include "networking/logic/storage/network_storage.h"
#include "networking/messages/network_message_gateway.h"
#include "networking/messages/network_messages_cartographer.h"
#include "networking/messages/network_messages_connect.h"
#include "networking/messages/network_messages_out_of_band.h"
#include "networking/messages/network_messages_session_membership.h"
#include "networking/messages/network_messages_session_parameters.h"
#include "networking/messages/network_messages_session_protocol.h"
#include "networking/messages/network_messages_simulation.h"
#include "networking/messages/network_messages_simulation_distributed.h"
#include "networking/messages/network_messages_simulation_synchronous.h"
#include "networking/messages/network_messages_test.h"
#include "networking/messages/network_messages_text_chat.h"
#include "networking/online/online_cheater.h"
#include "networking/online/online_strings.h"
#include "networking/online/voice_engine.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_manager.h"
#include "networking/session/network_text_chat_manager.h"
#include "transport/transport.h"

#include "shell/shell.h"

/* prototypes */

static void __cdecl network_globals_main_transition_initialize(void);

static void __cdecl network_globals_main_transition_dispose(void);

static void network_globals_unknown_transition_dispose(void);

/* public code */

void network_globals_apply_patches(void)
{
#ifdef EVENTS_ENABLED
	PatchCall(Memory::GetAddress(0x1B502A, 0x1A8D88), network_event_dispose);	// network_globals_dispose
#endif
	WritePointer((uintptr_t)&get_game_systems()[4], network_initialize);
	return;
}

s_network_globals* network_globals_get(void)
{
	return Memory::GetAddress<s_network_globals*>(0x51C45C, 0x520B7C);
}

c_network_link** global_network_link_get(void)
{
	return Memory::GetAddress<c_network_link**>(0x51C460, 0x520B80);
}

c_network_message_type_collection** global_network_message_types_get(void)
{
	return Memory::GetAddress<c_network_message_type_collection**>(0x51C464, 0x520B84);
}

c_network_message_gateway** global_network_message_gateway_get(void)
{
	return Memory::GetAddress<c_network_message_gateway**>(0x51C468, 0x520B88);
}

c_network_message_handler** global_network_message_handler_get(void)
{
	return Memory::GetAddress<c_network_message_handler**>(0x51C46C, 0x520B8C);
}

c_network_observer** global_network_observer_get(void)
{
	return Memory::GetAddress<c_network_observer**>(0x51C470, 0x520B90);
}

c_network_session** global_network_sessions_get(void)
{
	return Memory::GetAddress<c_network_session**>(0x51C474, 0x520B94);
}

c_network_session_manager** global_network_session_manager_get(void)
{
	return Memory::GetAddress<c_network_session_manager**>(0x51C478, 0x520B98);
}

c_network_text_chat_manager** global_network_text_chat_manager_get(void)
{
	return Memory::GetAddress<c_network_text_chat_manager**>(0x51C47C, 0x520B9C);
}

void network_initialize(void)
{
	s_network_globals* network_globals = network_globals_get();
	if (shell_application_type() == _shell_application_game && !network_globals->network_initialized)
	{
		if (shell_command_line_flag_is_set(_shell_command_line_flag_disable_voice_chat))
		{
			*Memory::GetAddress<bool*>(0x42242C, 0x3C5550) = false;
		}
		// shell_command_line_flag_is_set(_shell_command_line_flag_unk27);
		
		network_configuration_initialize();
#ifdef EVENTS_ENABLED
		network_event_initialize();
#endif
		if (network_memory_base_initialize(
			global_network_link_get(),
			global_network_message_types_get(),
			global_network_message_gateway_get(),
			global_network_message_handler_get(),
			global_network_observer_get(),
			global_network_sessions_get(),
			global_network_session_manager_get(),
			global_network_text_chat_manager_get()))
		{
			c_network_link* g_network_link = *global_network_link_get();
			c_network_message_type_collection* g_network_message_types = *global_network_message_types_get();
			c_network_message_gateway* g_network_message_gateway = *global_network_message_gateway_get();
			c_network_message_handler* g_network_message_handler = *global_network_message_handler_get();
			c_network_observer* g_network_observer = *global_network_observer_get();
			c_network_session* g_network_sessions = *global_network_sessions_get();
			c_network_session_manager* g_network_session_manager = *global_network_session_manager_get();
			c_network_text_chat_manager* g_network_text_chat_manager = *global_network_text_chat_manager_get();

			ASSERT(g_network_link != NULL);
			ASSERT(g_network_message_types != NULL);
			ASSERT(g_network_message_gateway != NULL);
			ASSERT(g_network_message_handler != NULL);
			ASSERT(g_network_observer != NULL);
			ASSERT(g_network_sessions != NULL);
			ASSERT(g_network_session_manager != NULL);
			ASSERT(g_network_text_chat_manager != NULL);


			const bool link_initialized = g_network_link->initialize_link();
			g_network_message_types->clear_message_types();
			network_message_types_register_out_of_band(g_network_message_types);
			network_message_types_register_connect(g_network_message_types);
			network_message_types_register_session_protocol(g_network_message_types);
			network_message_types_register_session_membership(g_network_message_types);
			network_message_types_register_session_parameters(g_network_message_types);
			network_message_types_register_simulation(g_network_message_types);
			network_message_types_register_simulation_synchronous(g_network_message_types);
			network_message_types_register_simulation_distributed(g_network_message_types);
			network_message_types_register_text_chat(g_network_message_types);
			network_message_types_register_test(g_network_message_types);
			network_message_types_register_cartographer_types(g_network_message_types);
		
			const bool observer_initialized =
				link_initialized
				&& g_network_message_gateway->initialize_gateway(g_network_link, g_network_message_types)
				&& g_network_message_handler->initialize_handler(g_network_link, g_network_message_types, g_network_message_gateway)
				&& g_network_observer->initialize_observer(g_network_link, g_network_message_types, g_network_message_gateway, g_network_observer_configuration);
			g_network_message_handler->register_observer(g_network_observer);

			const bool session_initialized =
				observer_initialized
				&& g_network_text_chat_manager->initialize_text_chat_manager()
				&& g_network_session_manager->initialize_session_manager();

			network_session_time_register_session_manager(g_network_session_manager);
			g_network_message_handler->register_session_manager(g_network_session_manager);

			const bool network_initialized =
				session_initialized
				&& g_network_sessions[0].initialize_session(0, _network_session_type_one, 0, g_network_message_gateway, g_network_observer, g_network_session_manager, g_network_text_chat_manager)
				&& g_network_sessions[1].initialize_session(1, _network_session_type_one, 1, g_network_message_gateway, g_network_observer, g_network_session_manager, g_network_text_chat_manager)
				&& network_broadcast_search_initialize(g_network_link, g_network_message_gateway)
				&& network_search_initialize()
				&& network_life_cycle_initialize(g_network_message_gateway, g_network_observer, g_network_session_manager, &g_network_sessions[0], &g_network_sessions[1])
				&& network_session_interface_initialize(g_network_session_manager)
				&& network_bandwidth_initialize(g_network_observer, &global_network_configuration_get()->gap_10)
				&& network_online_cheater_initialize();
			network_locator_initialize();

			if (network_initialized)
			{
				transport_register_transition_functions(network_globals_main_transition_initialize, network_globals_main_transition_dispose, NULL, NULL);
				transport_register_transition_functions(NULL, network_globals_unknown_transition_dispose, NULL, NULL);
				online_strings_initialize();
				network_storage_initialize();
				voice_engine_initialize();
				network_globals->network_initialized = true;
			}
			else
			{
				error(_error_delayed, "%s(): failed to initialize networking", __FUNCTION__);
				network_dispose();
			}
		}
		else
		{
			error(_error_delayed, "%s(): failed to initialize base networking memory layer", __FUNCTION__);
		}
	}
	return;
}

void network_dispose(void)
{
	INVOKE(0x1B4F46, 0x1A8CA4, network_dispose);
	return;
}

void __cdecl network_receive(void)
{
	INVOKE(0x1B542F, 0x1A918D, network_receive);
	return;
}

void __cdecl network_send(void)
{
	INVOKE(0x1B5456, 0x1A91B4, network_send);
	return;
}

void __cdecl network_update(void)
{
	INVOKE(0x1B53C7, 0x1A9125, network_update);
	return;
}

/* private code */

static void __cdecl network_globals_main_transition_initialize(void)
{
	INVOKE(0x1B5044, 0x1A8DA2, network_globals_main_transition_initialize);
	return;
}

static void __cdecl network_globals_main_transition_dispose(void)
{
	INVOKE(0x1B505D, 0x1A8DBB, network_globals_main_transition_dispose);
	return;
}

static void network_globals_unknown_transition_dispose(void)
{
	// Never completed?
	return;
}
