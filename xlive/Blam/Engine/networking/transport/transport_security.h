#pragma once

/* structures */

struct s_transport_secure_key
{
	XNKEY key;
};

struct s_transport_secure_identifier
{
	XNKID id;
};

struct s_transport_secure_address
{
	XNADDR addr;
};

struct s_transport_unique_identifier
{
	uint8 ab[6];
};

struct s_transport_session_description
{
	s_transport_secure_identifier id;
	s_transport_secure_key key;
	s_transport_secure_address host_address;
};

/* prototypes */

// Return true if both secure addresses are the same
bool transport_secure_address_compare(const s_transport_secure_address* a, const s_transport_secure_address* b);

char* transport_secure_identifier_get_string(const s_transport_secure_identifier* identifier);

const char* managed_session_get_id_string(const s_transport_secure_identifier* session_id);

const char* transport_unique_identifier_get_string(const s_transport_unique_identifier* unique_identifier);

char* transport_secure_address_to_string(const s_transport_secure_address* in_secure_address, char* string, int32 size, bool include_online_address, bool include_mac_address);

const char* transport_secure_address_get_mac_string(const s_transport_secure_address* address);

bool __cdecl transport_secure_identifier_retrieve(
	const struct transport_address* usable_address,
	enum e_transport_platform platform,
	int32* remote_identifier,
	struct s_transport_secure_identifier* secure_identifier,
	struct s_transport_secure_key* secure_key,
	struct s_transport_secure_address* secure_address);

bool __cdecl transport_secure_address_get(struct s_transport_secure_address* secure_address, struct transport_address* address);
