#include "stdafx.h"
#include "kablam_rpc_client.h"

#include <cwchar>

#include "kablam_rpc/kablam_commands_h.h"
#include "kablam_rpc/kablam_rpc.h"

static RPC_WSTR local_rpc_protocol = (RPC_WSTR)L"ncalrpc";
const wchar_t* const g_kablam_rpc_endpoint_prefix = L"H2SERVER_LRPC";

kablam_rpc_client* g_kablam_rpc_client;

void kablam_rpc_client::build_instance_endpoint(wchar_t* instance_name)
{
    if (instance_name)
        _snwprintf_s(this->m_rpc_endpoint, 27, _TRUNCATE, L"%s_%s", g_kablam_rpc_endpoint_prefix, instance_name);
    else
        wcsncpy_s(this->m_rpc_endpoint, 27, g_kablam_rpc_endpoint_prefix, _TRUNCATE);
}

RPC_STATUS kablam_rpc_client::build_binding_string_local()
{
    return RpcStringBindingComposeW(
        nullptr,
        local_rpc_protocol,
        nullptr,
        (RPC_WSTR)this->m_rpc_endpoint,
        nullptr,
        &this->m_rpc_binding_string
    );
}

RPC_STATUS kablam_rpc_client::set_auth_info_local(wchar_t* instance_username)
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

bool kablam_rpc_client::connect_local(wchar_t* instance_name, wchar_t* instance_username, kablam_rpc_service_version_info* out_version_info)
{
    if (!instance_name)
    {
        // ASSERT();
        return false;
    }

    this->build_instance_endpoint(instance_name);

    RPC_STATUS status;

    this->m_last_status = this->build_binding_string_local();

    if (this->m_last_status)
    {
        // ASSERT();
        return false;
    }

    this->m_last_status = RpcBindingFromStringBindingW(this->m_rpc_binding_string, &kablam_commands_IfHandle);

    if (this->m_last_status)
    {
        // ASSERT();
        return false;
    }

    //
    // Turns out the security binding isn't required to connect to the RPC Session?
    //

    //status = this->set_auth_info_local(instance_username);

    //if (this->m_last_status)
    //{
    // // ASSERT();
    //    return false;
    //}

    RpcTryExcept
    {
        kablam_command_get_version_rpc(4, out_version_info->unknown_version, 32, out_version_info->build_string, &out_version_info->build_version);
		return true;
    }
    RpcExcept(1)
    {
        return false;
    }
    RpcEndExcept

    return false;
}

void kablam_rpc_client::disconnect_local()
{
    RpcBindingFree(&kablam_commands_IfHandle);
}

void kablam_rpc_client_initialize()
{
    g_kablam_rpc_client = new kablam_rpc_client();
}

void kablam_rpc_client_free()
{
    delete g_kablam_rpc_client;
}

kablam_rpc_client* kablam_rpc_client_get()
{
    return g_kablam_rpc_client;
}
