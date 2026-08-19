#pragma once

/* classes */

class kablam_rpc_client
{
public:
	RPC_STATUS m_last_status;
	wchar_t m_rpc_endpoint[27]{};
	RPC_WSTR m_rpc_binding_string{};

	void build_instance_endpoint(wchar_t const* instance_name);
	RPC_STATUS build_binding_string_local(void);
	RPC_STATUS set_auth_info_local(wchar_t const* instance_username);

public:
	bool connect_local(wchar_t const* instance_name, wchar_t const* instance_username, struct kablam_rpc_service_version_info* out_version_info);
	//bool connect_tcp_ip(wchar_t* host, wchar_t* port, wchar_t* username, wchar_t* password, kablam_rpc_service_version_info* out_version_info);

	void disconnect_local(void);
	//void disconnect_tcp_ip();
};

/* prototypes */

void kablam_rpc_client_initialize(void);

void kablam_rpc_client_free(void);

kablam_rpc_client* kablam_rpc_client_get(void);
