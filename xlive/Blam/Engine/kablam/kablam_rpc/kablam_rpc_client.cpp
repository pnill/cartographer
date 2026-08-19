#include "stdafx.h"
#include "kablam_rpc_client.h"

#include "kablam_rpc/kablam_commands_h.h"
#include "kablam_rpc/kablam_rpc.h"

/* constants */

static const RPC_WSTR k_local_rpc_protocol = (RPC_WSTR)L"ncalrpc";
static wchar_t const* const k_kablam_rpc_endpoint_prefix = L"H2SERVER_LRPC";

/* globals */

static kablam_rpc_client* g_kablam_rpc_client;

/* public code */

void kablam_rpc_client::build_instance_endpoint(
	wchar_t const* instance_name)
{
	if (instance_name)
	{
		_snwprintf_s(this->m_rpc_endpoint, NUMBEROF(m_rpc_endpoint), _TRUNCATE, L"%s_%s", k_kablam_rpc_endpoint_prefix, instance_name);
	}
	else
	{
		wcsncpy_s(this->m_rpc_endpoint, NUMBEROF(m_rpc_endpoint), k_kablam_rpc_endpoint_prefix, _TRUNCATE);
	}
	return;
}

RPC_STATUS kablam_rpc_client::build_binding_string_local(void)
{
	return RpcStringBindingComposeW(
		nullptr,
		k_local_rpc_protocol,
		nullptr,
		(RPC_WSTR)this->m_rpc_endpoint,
		nullptr,
		&this->m_rpc_binding_string
	);
}

RPC_STATUS kablam_rpc_client::set_auth_info_local(
	wchar_t const* instance_username)
{
	RPC_SECURITY_QOS security_qos{};
	security_qos.Version = RPC_C_SECURITY_QOS_VERSION;
	security_qos.ImpersonationType = RPC_C_IMP_LEVEL_IMPERSONATE;
	security_qos.IdentityTracking = RPC_C_QOS_IDENTITY_STATIC;
	security_qos.Capabilities = RPC_C_QOS_CAPABILITIES_DEFAULT;

	return RpcBindingSetAuthInfoExW(
		kablam_commands_IfHandle,
		(RPC_WSTR)instance_username,
		RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
		RPC_C_AUTHN_WINNT,
		nullptr,
		RPC_C_AUTHZ_NONE,
		&security_qos);
}

bool kablam_rpc_client::connect_local(
	wchar_t const* instance_name,
	wchar_t const* instance_username,
	kablam_rpc_service_version_info* out_version_info)
{
	UNREFERENCED_PARAMETER(instance_username);

	bool result = false;

	if (!instance_name)
	{
		assert(false && "instance name is nullptr");
		return false;
	}
	else
	{
		this->build_instance_endpoint(instance_name);

		RPC_STATUS status;
		UNREFERENCED_PARAMETER(status);

		this->m_last_status = this->build_binding_string_local();

		if (this->m_last_status)
		{
			assert(this->m_last_status && "RPC failed to build binding string");
			return false;
		}

		this->m_last_status = RpcBindingFromStringBindingW(this->m_rpc_binding_string, &kablam_commands_IfHandle);

		if (this->m_last_status)
		{
			assert(this->m_last_status && "RPC failed to binding");
			return false;
		}

		//
		// Turns out the security binding isn't required to connect to the RPC Session?
		//

		//status = this->set_auth_info_local(instance_username);

		//if (this->m_last_status)
		//{
		//	  assert(this->m_last_status && "RPC failed to set authentication info");
		//    return false;
		//}

		RpcTryExcept
		{
			kablam_command_get_version_rpc(4, out_version_info->unknown_version, 32, out_version_info->build_string, &out_version_info->build_version);
			result = true;
		}
		RpcExcept(1)
		{
		}
		RpcEndExcept
	}
	return result;
}

void kablam_rpc_client::disconnect_local(void)
{
	RpcBindingFree(&kablam_commands_IfHandle);
	return;
}

void kablam_rpc_client_initialize(void)
{
	g_kablam_rpc_client = new kablam_rpc_client();
	return;
}

void kablam_rpc_client_free(void)
{
	delete g_kablam_rpc_client;
	return;
}

kablam_rpc_client* kablam_rpc_client_get(void)
{
	return g_kablam_rpc_client;
}
