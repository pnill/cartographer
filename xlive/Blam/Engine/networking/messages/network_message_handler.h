#pragma once
#include "networking/transport/transport_security.h"

/* structures */

struct s_network_message_session_data
{
	s_transport_secure_identifier identifier;
};

struct s_network_message_leave_session
{
	s_network_message_session_data session_data;
};

/* classes */

class c_network_message_handler
{
public:
	bool initialize_handler(class c_network_link* link, const class c_network_message_type_collection* message_types, class c_network_message_gateway* message_gateway);
	void register_session_manager(class c_network_session_manager* session_manager);
	void register_observer(class c_network_observer* observer);
	
	/* cartographer message handlers */

	void handle_request_map_filename(const struct transport_address* address, const struct s_network_message_request_map_filename* received_data);
	void handle_map_filename_response(const struct transport_address* address, int32 channel_index, const struct s_network_message_custom_map_filename* received_data);
	void handle_player_property_rank(const struct transport_address* address, int32 channel_index, const struct s_network_message_rank_change* received_data);
	void handle_session_anticheat_status(const struct transport_address* address, int32 channel_index, const struct s_network_message_anti_cheat* received_data);

	void handle_leave_session(const struct transport_address* address, const struct s_network_message_session_data* received_data);
	void handle_membership_update(const struct transport_address* address, int32 channel_index, const struct s_network_message_session_data* received_data);
	void handle_player_add(const struct transport_address* address, const struct s_network_message_session_data* received_data);
private:
	bool m_initialized;
	class c_network_link* m_link;
	const class c_network_message_type_collection* m_message_types;
	class c_network_message_gateway* m_message_gateway;
	class c_network_observer* m_observer;
	class c_network_session_manager* m_session_manager;
};

/* prototypes */

void network_message_handler_apply_patches(void);