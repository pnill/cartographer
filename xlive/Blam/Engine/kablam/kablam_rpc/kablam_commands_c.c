

/* this ALWAYS GENERATED file contains the RPC client stubs */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for kablam_rpc\kablam_commands.idl:
    Oicf, W0, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, app_config, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)


#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/

#include <string.h>

#include "kablam_commands_h.h"

#define TYPE_FORMAT_STRING_SIZE   489                               
#define PROC_FORMAT_STRING_SIZE   1429                              
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _kablam_commands_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } kablam_commands_MIDL_TYPE_FORMAT_STRING;

typedef struct _kablam_commands_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } kablam_commands_MIDL_PROC_FORMAT_STRING;

typedef struct _kablam_commands_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } kablam_commands_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax_2_0 = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

static const RPC_SYNTAX_IDENTIFIER  _NDR64_RpcTransferSyntax_1_0 = 
{{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}};

#if defined(_CONTROL_FLOW_GUARD_XFG)
#define XFG_TRAMPOLINES(ObjectType)\
NDR_SHAREABLE unsigned long ObjectType ## _UserSize_XFG(unsigned long * pFlags, unsigned long Offset, void * pObject)\
{\
return  ObjectType ## _UserSize(pFlags, Offset, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserMarshal_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserMarshal(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserUnmarshal_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserUnmarshal(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE void ObjectType ## _UserFree_XFG(unsigned long * pFlags, void * pObject)\
{\
ObjectType ## _UserFree(pFlags, (ObjectType *)pObject);\
}
#define XFG_TRAMPOLINES64(ObjectType)\
NDR_SHAREABLE unsigned long ObjectType ## _UserSize64_XFG(unsigned long * pFlags, unsigned long Offset, void * pObject)\
{\
return  ObjectType ## _UserSize64(pFlags, Offset, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserMarshal64_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserMarshal64(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserUnmarshal64_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserUnmarshal64(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE void ObjectType ## _UserFree64_XFG(unsigned long * pFlags, void * pObject)\
{\
ObjectType ## _UserFree64(pFlags, (ObjectType *)pObject);\
}
#define XFG_BIND_TRAMPOLINES(HandleType, ObjectType)\
static void* ObjectType ## _bind_XFG(HandleType pObject)\
{\
return ObjectType ## _bind((ObjectType) pObject);\
}\
static void ObjectType ## _unbind_XFG(HandleType pObject, handle_t ServerHandle)\
{\
ObjectType ## _unbind((ObjectType) pObject, ServerHandle);\
}
#define XFG_TRAMPOLINE_FPTR(Function) Function ## _XFG
#define XFG_TRAMPOLINE_FPTR_DEPENDENT_SYMBOL(Symbol) Symbol ## _XFG
#else
#define XFG_TRAMPOLINES(ObjectType)
#define XFG_TRAMPOLINES64(ObjectType)
#define XFG_BIND_TRAMPOLINES(HandleType, ObjectType)
#define XFG_TRAMPOLINE_FPTR(Function) Function
#define XFG_TRAMPOLINE_FPTR_DEPENDENT_SYMBOL(Symbol) Symbol
#endif



extern const kablam_commands_MIDL_TYPE_FORMAT_STRING kablam_commands__MIDL_TypeFormatString;
extern const kablam_commands_MIDL_PROC_FORMAT_STRING kablam_commands__MIDL_ProcFormatString;
extern const kablam_commands_MIDL_EXPR_FORMAT_STRING kablam_commands__MIDL_ExprFormatString;

#define GENERIC_BINDING_TABLE_SIZE   0            


/* Standard interface: kablam_commands_interface, ver. 1.0,
   GUID={0x0a8c5d7f,0x2550,0x40da,{0xb3,0x57,0xe1,0xfd,0x83,0xdb,0xe3,0x93}} */

 extern const MIDL_STUBLESS_PROXY_INFO kablam_commands_interface_ProxyInfo;
handle_t kablam_commands_IfHandle;


static const RPC_CLIENT_INTERFACE kablam_commands_interface___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x0a8c5d7f,0x2550,0x40da,{0xb3,0x57,0xe1,0xfd,0x83,0xdb,0xe3,0x93}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    &kablam_commands_interface_ProxyInfo,
    0x02000000
    };
RPC_IF_HANDLE kablam_commands_interface_v1_0_c_ifspec = (RPC_IF_HANDLE)& kablam_commands_interface___RpcClientInterface;
#ifdef __cplusplus
namespace {
#endif

extern const MIDL_STUB_DESC kablam_commands_interface_StubDesc;
#ifdef __cplusplus
}
#endif

static RPC_BINDING_HANDLE kablam_commands_interface__MIDL_AutoBindHandle;


void kablam_command_get_version_rpc( 
    /* [in] */ long unk_version_length,
    /* [size_is][string][out] */ unsigned char *unk_version,
    /* [in] */ long build_string_length,
    /* [size_is][string][out] */ unsigned char *build_string,
    /* [out] */ long *version_int)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  0,
                  0,
                  unk_version_length,
                  unk_version,
                  build_string_length,
                  build_string,
                  version_int);
    
}


void kablam_command_kill_server_rpc( void)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  1,
                  0,
                  0);
    
}


void kablam_command_skip_rpc( 
    /* [out] */ struct kablam_command_skip_result *result)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  2,
                  0,
                  result);
    
}


void kablam_command_kick_rpc( 
    /* [string][in] */ wchar_t *gamer_tag,
    /* [ref][out] */ kablam_command_kick_result_code *result)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  3,
                  0,
                  gamer_tag,
                  result);
    
}


void kablam_command_status_rpc( 
    /* [out] */ struct kablam_command_status_result *result)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  4,
                  0,
                  result);
    
}


void kablam_command_get_name_rpc( 
    /* [ref][out] */ e_kablam_command_set_unicode_setting_result_code *result_code,
    /* [in] */ long buffer_length,
    /* [size_is][string][out] */ wchar_t *out_name)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  5,
                  0,
                  result_code,
                  buffer_length,
                  out_name);
    
}


void kablam_command_set_name_rpc( 
    /* [string][in] */ wchar_t *name,
    /* [ref][out] */ e_kablam_command_set_unicode_setting_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  6,
                  0,
                  name,
                  result_code);
    
}


void kablam_command_get_description_rpc( 
    /* [ref][out] */ e_kablam_command_set_unicode_setting_result_code *result_code,
    /* [in] */ long buffer_length,
    /* [size_is][string][out] */ wchar_t *out_description)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  7,
                  0,
                  result_code,
                  buffer_length,
                  out_description);
    
}


void kablam_command_set_description_rpc( 
    /* [string][in] */ wchar_t *description,
    /* [ref][out] */ e_kablam_command_set_unicode_setting_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  8,
                  0,
                  description,
                  result_code);
    
}


void kablam_command_send_message_rpc( 
    /* [size_is][string][in] */ wchar_t *message,
    /* [ref][out] */ e_kablam_command_send_message_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  9,
                  0,
                  message,
                  result_code);
    
}


void kablam_command_play_rpc( 
    /* [string][in] */ wchar_t *file_path,
    /* [out] */ struct kablam_command_play_result *out_response)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  10,
                  0,
                  file_path,
                  out_response);
    
}


void kablam_command_playing_rpc( 
    /* [out] */ struct kablam_command_play_result *out_response)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  11,
                  0,
                  out_response);
    
}


void kablam_command_live_activate_rpc( 
    /* [string][in] */ wchar_t *key,
    /* [ref][out] */ e_kablam_command_live_key_response_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  12,
                  0,
                  key,
                  result_code);
    
}


void kablam_command_live_auto_signin_rpc( 
    /* [string][in] */ wchar_t *username,
    /* [string][in] */ wchar_t *password,
    /* [ref][out] */ e_kablam_command_live_auto_signin_response_code *result_code,
    /* [out] */ unsigned long *xlive_login_result)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  13,
                  0,
                  username,
                  password,
                  result_code,
                  xlive_login_result);
    
}


void kablam_command_live_signin_rpc( 
    /* [string][in] */ wchar_t *username,
    /* [string][in] */ wchar_t *password,
    /* [ref][out] */ e_kablam_command_live_signin_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  14,
                  0,
                  username,
                  password,
                  result_code);
    
}


void kablam_command_live_signout_rpc( 
    /* [ref][out] */ e_kablam_command_live_signout_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  15,
                  0,
                  result_code);
    
}


void kablam_command_ban_ip_rpc( 
    /* [in] */ long ipv4_address,
    /* [in] */ unsigned char subnet,
    /* [in] */ long duration,
    /* [ref][out] */ e_kablam_command_ban_network_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  16,
                  0,
                  ipv4_address,
                  subnet,
                  duration,
                  result_code);
    
}


void kablam_command_ban_nic_rpc( 
    /* [in] */ struct s_mac_address *mac_address,
    /* [in] */ long duration,
    /* [ref][out] */ e_kablam_command_ban_network_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  17,
                  0,
                  mac_address,
                  duration,
                  result_code);
    
}


void kablam_command_ban_gamer_rpc( 
    /* [string][in] */ wchar_t *gamer_tag,
    /* [in] */ long duration,
    /* [ref][out] */ e_kablam_command_gamer_table_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  18,
                  0,
                  gamer_tag,
                  duration,
                  result_code);
    
}


void kablam_command_unban_ip_rpc( 
    /* [in] */ long ipv4_address,
    /* [in] */ unsigned char cidr,
    /* [ref][out] */ e_kablam_command_ban_network_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  19,
                  0,
                  ipv4_address,
                  cidr,
                  result_code);
    
}


void kablam_command_unban_nic_rpc( 
    /* [in] */ struct s_mac_address *mac_address,
    /* [ref][out] */ e_kablam_command_ban_network_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  20,
                  0,
                  mac_address,
                  result_code);
    
}


void kablam_command_unban_gamer_rpc( 
    /* [string][in] */ wchar_t *gamer_tag,
    /* [ref][out] */ e_kablam_command_gamer_table_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  21,
                  0,
                  gamer_tag,
                  result_code);
    
}


void kablam_command_unban_all_rpc( 
    /* [in] */ e_kablam_unban_all_type unban_all_type,
    /* [ref][out] */ e_kablam_command_unban_all_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  22,
                  0,
                  unban_all_type,
                  result_code);
    
}


void kablam_command_get_ban_ip_rpc( 
    /* [out] */ unsigned long *returned_results_count,
    /* [out] */ long *returned_results_size,
    /* [size_is][size_is][ref][out] */ struct s_exipration_entry_ipv4_subnet **buffer)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  23,
                  0,
                  returned_results_count,
                  returned_results_size,
                  buffer);
    
}


void kablam_command_get_ban_nic_rpc( 
    /* [out] */ unsigned long *returned_results_count,
    /* [out] */ long *returned_results_size,
    /* [size_is][size_is][ref][out] */ struct s_expiration_entry_mac_address **buffer)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  24,
                  0,
                  returned_results_count,
                  returned_results_size,
                  buffer);
    
}


void kablam_command_get_ban_gamer_rpc( 
    /* [out] */ unsigned long *returned_results_count,
    /* [out] */ long *returned_results_size,
    /* [size_is][size_is][ref][out] */ struct s_expiration_entry_gamer **buffer)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  25,
                  0,
                  returned_results_count,
                  returned_results_size,
                  buffer);
    
}


void kablam_command_vip_add_rpc( 
    /* [string][in] */ wchar_t *gamer_tag,
    /* [ref][out] */ e_kablam_command_gamer_table_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  26,
                  0,
                  gamer_tag,
                  result_code);
    
}


void kablam_command_vip_remove_rpc( 
    /* [string][in] */ wchar_t *gamer_tag,
    /* [ref][out] */ e_kablam_command_gamer_table_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  27,
                  0,
                  gamer_tag,
                  result_code);
    
}


void kablam_command_vip_clear_rpc( 
    /* [ref][out] */ e_kablam_command_vip_clear_result_code *arg_1)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  28,
                  0,
                  arg_1);
    
}


void kablam_command_get_vip_rpc( 
    /* [out] */ unsigned long *returned_results_count,
    /* [out] */ long *returned_results_size,
    /* [size_is][size_is][ref][out] */ struct s_gamertag **buffer)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  29,
                  0,
                  returned_results_count,
                  returned_results_size,
                  buffer);
    
}


void kablam_command_get_stats_folder_rpc( 
    /* [ref][out] */ e_kablam_command_stats_folder_result_code *result_code,
    /* [in] */ long buffer_length,
    /* [size_is][string][out] */ wchar_t *out_path)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  30,
                  0,
                  result_code,
                  buffer_length,
                  out_path);
    
}


void kablam_command_set_stats_folder_rpc( 
    /* [string][in] */ wchar_t *in_path,
    /* [ref][out] */ e_kablam_command_stats_folder_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  31,
                  0,
                  in_path,
                  result_code);
    
}


void kablam_command_set_privacy_rpc( 
    /* [in] */ e_kablam_command_privacy_type privacy_type,
    /* [ref][out] */ e_kablam_command_privacy_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  32,
                  0,
                  privacy_type,
                  result_code);
    
}


void kablam_command_get_privacy_rpc( 
    /* [ref][out] */ e_kablam_command_privacy_type *out_privacy,
    /* [ref][out] */ e_kablam_command_privacy_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  33,
                  0,
                  out_privacy,
                  result_code);
    
}


void kablam_command_set_max_players_rpc( 
    /* [in] */ unsigned int player_count,
    /* [ref][out] */ e_kablam_command_set_max_players_result_code *result_code)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  34,
                  0,
                  player_count,
                  result_code);
    
}


void kablam_command_get_max_players_rpc( 
    /* [out] */ unsigned long *out_player_count)
{

    NdrClientCall3(
                  ( PMIDL_STUBLESS_PROXY_INFO  )&kablam_commands_interface_ProxyInfo,
                  35,
                  0,
                  out_player_count);
    
}


#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const kablam_commands_MIDL_PROC_FORMAT_STRING kablam_commands__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure kablam_command_get_version_rpc */

			0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 10 */	NdrFcShort( 0x10 ),	/* 16 */
/* 12 */	NdrFcShort( 0x1c ),	/* 28 */
/* 14 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x5,		/* 5 */
/* 16 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 18 */	NdrFcShort( 0x1 ),	/* 1 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter unk_version_length */

/* 26 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 28 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter unk_version */

/* 32 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 34 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 36 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter build_string_length */

/* 38 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 40 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 42 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter build_string */

/* 44 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 46 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 48 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Parameter version_int */

/* 50 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 52 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure kablam_command_kill_server_rpc */

/* 56 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 58 */	NdrFcLong( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x1 ),	/* 1 */
/* 64 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	0x40,		/* Oi2 Flags:  has ext, */
			0x0,		/* 0 */
/* 72 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0x0 ),	/* 0 */
/* 78 */	NdrFcShort( 0x0 ),	/* 0 */
/* 80 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Procedure kablam_command_skip_rpc */

/* 82 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 84 */	NdrFcLong( 0x0 ),	/* 0 */
/* 88 */	NdrFcShort( 0x2 ),	/* 2 */
/* 90 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 92 */	NdrFcShort( 0x0 ),	/* 0 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x1,		/* 1 */
/* 98 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter result */

/* 108 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 110 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 112 */	NdrFcShort( 0x28 ),	/* Type Offset=40 */

	/* Procedure kablam_command_kick_rpc */

/* 114 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 116 */	NdrFcLong( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0x3 ),	/* 3 */
/* 122 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x1a ),	/* 26 */
/* 128 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x2,		/* 2 */
/* 130 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter gamer_tag */

/* 140 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 142 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 144 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter result */

/* 146 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 148 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 150 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_status_rpc */

/* 152 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 154 */	NdrFcLong( 0x0 ),	/* 0 */
/* 158 */	NdrFcShort( 0x4 ),	/* 4 */
/* 160 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 166 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x1,		/* 1 */
/* 168 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter result */

/* 178 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 180 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 182 */	NdrFcShort( 0x6a ),	/* Type Offset=106 */

	/* Procedure kablam_command_get_name_rpc */

/* 184 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 186 */	NdrFcLong( 0x0 ),	/* 0 */
/* 190 */	NdrFcShort( 0x5 ),	/* 5 */
/* 192 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 194 */	NdrFcShort( 0x8 ),	/* 8 */
/* 196 */	NdrFcShort( 0x1a ),	/* 26 */
/* 198 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x3,		/* 3 */
/* 200 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 202 */	NdrFcShort( 0x1 ),	/* 1 */
/* 204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter result_code */

/* 210 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 212 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 214 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter buffer_length */

/* 216 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 218 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_name */

/* 222 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 224 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 226 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Procedure kablam_command_set_name_rpc */

/* 228 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 230 */	NdrFcLong( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x6 ),	/* 6 */
/* 236 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0x1a ),	/* 26 */
/* 242 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x2,		/* 2 */
/* 244 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 246 */	NdrFcShort( 0x0 ),	/* 0 */
/* 248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 252 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter name */

/* 254 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 256 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 258 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter result_code */

/* 260 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 262 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 264 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_get_description_rpc */

/* 266 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 268 */	NdrFcLong( 0x0 ),	/* 0 */
/* 272 */	NdrFcShort( 0x7 ),	/* 7 */
/* 274 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 276 */	NdrFcShort( 0x8 ),	/* 8 */
/* 278 */	NdrFcShort( 0x1a ),	/* 26 */
/* 280 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x3,		/* 3 */
/* 282 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 284 */	NdrFcShort( 0x1 ),	/* 1 */
/* 286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter result_code */

/* 292 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 294 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 296 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter buffer_length */

/* 298 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 300 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 302 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_description */

/* 304 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 306 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 308 */	NdrFcShort( 0xb6 ),	/* Type Offset=182 */

	/* Procedure kablam_command_set_description_rpc */

/* 310 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 312 */	NdrFcLong( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x8 ),	/* 8 */
/* 318 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0x1a ),	/* 26 */
/* 324 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x2,		/* 2 */
/* 326 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter description */

/* 336 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 338 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 340 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter result_code */

/* 342 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 344 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 346 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_send_message_rpc */

/* 348 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 350 */	NdrFcLong( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0x9 ),	/* 9 */
/* 356 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x1a ),	/* 26 */
/* 362 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x2,		/* 2 */
/* 364 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 368 */	NdrFcShort( 0x1 ),	/* 1 */
/* 370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 372 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter message */

/* 374 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 376 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 378 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Parameter result_code */

/* 380 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 382 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 384 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_play_rpc */

/* 386 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 388 */	NdrFcLong( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0xa ),	/* 10 */
/* 394 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 400 */	0x43,		/* Oi2 Flags:  srv must size, clt must size, has ext, */
			0x2,		/* 2 */
/* 402 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 410 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter file_path */

/* 412 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 414 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 416 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter out_response */

/* 418 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 420 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 422 */	NdrFcShort( 0xfc ),	/* Type Offset=252 */

	/* Procedure kablam_command_playing_rpc */

/* 424 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 426 */	NdrFcLong( 0x0 ),	/* 0 */
/* 430 */	NdrFcShort( 0xb ),	/* 11 */
/* 432 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 436 */	NdrFcShort( 0x0 ),	/* 0 */
/* 438 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x1,		/* 1 */
/* 440 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 448 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter out_response */

/* 450 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 452 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 454 */	NdrFcShort( 0xfc ),	/* Type Offset=252 */

	/* Procedure kablam_command_live_activate_rpc */

/* 456 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 458 */	NdrFcLong( 0x0 ),	/* 0 */
/* 462 */	NdrFcShort( 0xc ),	/* 12 */
/* 464 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 468 */	NdrFcShort( 0x1a ),	/* 26 */
/* 470 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x2,		/* 2 */
/* 472 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 480 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter key */

/* 482 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 484 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 486 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter result_code */

/* 488 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 490 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 492 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_live_auto_signin_rpc */

/* 494 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 496 */	NdrFcLong( 0x0 ),	/* 0 */
/* 500 */	NdrFcShort( 0xd ),	/* 13 */
/* 502 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 506 */	NdrFcShort( 0x36 ),	/* 54 */
/* 508 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x4,		/* 4 */
/* 510 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 512 */	NdrFcShort( 0x0 ),	/* 0 */
/* 514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 518 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter username */

/* 520 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 522 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 524 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter password */

/* 526 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 528 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 530 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter result_code */

/* 532 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 534 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 536 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter xlive_login_result */

/* 538 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 540 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure kablam_command_live_signin_rpc */

/* 544 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 546 */	NdrFcLong( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0xe ),	/* 14 */
/* 552 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 556 */	NdrFcShort( 0x1a ),	/* 26 */
/* 558 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x3,		/* 3 */
/* 560 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 568 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter username */

/* 570 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 572 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 574 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter password */

/* 576 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 578 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 580 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter result_code */

/* 582 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 584 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 586 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_live_signout_rpc */

/* 588 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 590 */	NdrFcLong( 0x0 ),	/* 0 */
/* 594 */	NdrFcShort( 0xf ),	/* 15 */
/* 596 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 598 */	NdrFcShort( 0x0 ),	/* 0 */
/* 600 */	NdrFcShort( 0x1a ),	/* 26 */
/* 602 */	0x40,		/* Oi2 Flags:  has ext, */
			0x1,		/* 1 */
/* 604 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 606 */	NdrFcShort( 0x0 ),	/* 0 */
/* 608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 612 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter result_code */

/* 614 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 616 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 618 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_ban_ip_rpc */

/* 620 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 622 */	NdrFcLong( 0x0 ),	/* 0 */
/* 626 */	NdrFcShort( 0x10 ),	/* 16 */
/* 628 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 630 */	NdrFcShort( 0x15 ),	/* 21 */
/* 632 */	NdrFcShort( 0x1a ),	/* 26 */
/* 634 */	0x40,		/* Oi2 Flags:  has ext, */
			0x4,		/* 4 */
/* 636 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 638 */	NdrFcShort( 0x0 ),	/* 0 */
/* 640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 642 */	NdrFcShort( 0x0 ),	/* 0 */
/* 644 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ipv4_address */

/* 646 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 648 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 650 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subnet */

/* 652 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 654 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 656 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter duration */

/* 658 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 660 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 662 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter result_code */

/* 664 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 666 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 668 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_ban_nic_rpc */

/* 670 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 672 */	NdrFcLong( 0x0 ),	/* 0 */
/* 676 */	NdrFcShort( 0x11 ),	/* 17 */
/* 678 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 680 */	NdrFcShort( 0x42 ),	/* 66 */
/* 682 */	NdrFcShort( 0x1a ),	/* 26 */
/* 684 */	0x40,		/* Oi2 Flags:  has ext, */
			0x3,		/* 3 */
/* 686 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 694 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mac_address */

/* 696 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 698 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 700 */	NdrFcShort( 0x11c ),	/* Type Offset=284 */

	/* Parameter duration */

/* 702 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 704 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 706 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter result_code */

/* 708 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 710 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 712 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_ban_gamer_rpc */

/* 714 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 716 */	NdrFcLong( 0x0 ),	/* 0 */
/* 720 */	NdrFcShort( 0x12 ),	/* 18 */
/* 722 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 724 */	NdrFcShort( 0x8 ),	/* 8 */
/* 726 */	NdrFcShort( 0x1a ),	/* 26 */
/* 728 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x3,		/* 3 */
/* 730 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter gamer_tag */

/* 740 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 742 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 744 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter duration */

/* 746 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 748 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 750 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter result_code */

/* 752 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 754 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 756 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_unban_ip_rpc */

/* 758 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 760 */	NdrFcLong( 0x0 ),	/* 0 */
/* 764 */	NdrFcShort( 0x13 ),	/* 19 */
/* 766 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 768 */	NdrFcShort( 0xd ),	/* 13 */
/* 770 */	NdrFcShort( 0x1a ),	/* 26 */
/* 772 */	0x40,		/* Oi2 Flags:  has ext, */
			0x3,		/* 3 */
/* 774 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 778 */	NdrFcShort( 0x0 ),	/* 0 */
/* 780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 782 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ipv4_address */

/* 784 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 786 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 788 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cidr */

/* 790 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 792 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 794 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter result_code */

/* 796 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 798 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 800 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_unban_nic_rpc */

/* 802 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 804 */	NdrFcLong( 0x0 ),	/* 0 */
/* 808 */	NdrFcShort( 0x14 ),	/* 20 */
/* 810 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 812 */	NdrFcShort( 0x3a ),	/* 58 */
/* 814 */	NdrFcShort( 0x1a ),	/* 26 */
/* 816 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 818 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 822 */	NdrFcShort( 0x0 ),	/* 0 */
/* 824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 826 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mac_address */

/* 828 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 830 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 832 */	NdrFcShort( 0x11c ),	/* Type Offset=284 */

	/* Parameter result_code */

/* 834 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 836 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 838 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_unban_gamer_rpc */

/* 840 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 842 */	NdrFcLong( 0x0 ),	/* 0 */
/* 846 */	NdrFcShort( 0x15 ),	/* 21 */
/* 848 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 852 */	NdrFcShort( 0x1a ),	/* 26 */
/* 854 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x2,		/* 2 */
/* 856 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 858 */	NdrFcShort( 0x0 ),	/* 0 */
/* 860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 862 */	NdrFcShort( 0x0 ),	/* 0 */
/* 864 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter gamer_tag */

/* 866 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 868 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 870 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter result_code */

/* 872 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 874 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 876 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_unban_all_rpc */

/* 878 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 880 */	NdrFcLong( 0x0 ),	/* 0 */
/* 884 */	NdrFcShort( 0x16 ),	/* 22 */
/* 886 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 888 */	NdrFcShort( 0x6 ),	/* 6 */
/* 890 */	NdrFcShort( 0x1a ),	/* 26 */
/* 892 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 894 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 896 */	NdrFcShort( 0x0 ),	/* 0 */
/* 898 */	NdrFcShort( 0x0 ),	/* 0 */
/* 900 */	NdrFcShort( 0x0 ),	/* 0 */
/* 902 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter unban_all_type */

/* 904 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 906 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 908 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter result_code */

/* 910 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 912 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 914 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_get_ban_ip_rpc */

/* 916 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 918 */	NdrFcLong( 0x0 ),	/* 0 */
/* 922 */	NdrFcShort( 0x17 ),	/* 23 */
/* 924 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 926 */	NdrFcShort( 0x0 ),	/* 0 */
/* 928 */	NdrFcShort( 0x38 ),	/* 56 */
/* 930 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x3,		/* 3 */
/* 932 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 934 */	NdrFcShort( 0x1 ),	/* 1 */
/* 936 */	NdrFcShort( 0x0 ),	/* 0 */
/* 938 */	NdrFcShort( 0x0 ),	/* 0 */
/* 940 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter returned_results_count */

/* 942 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 944 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter returned_results_size */

/* 948 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 950 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 952 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter buffer */

/* 954 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 956 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 958 */	NdrFcShort( 0x126 ),	/* Type Offset=294 */

	/* Procedure kablam_command_get_ban_nic_rpc */

/* 960 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 962 */	NdrFcLong( 0x0 ),	/* 0 */
/* 966 */	NdrFcShort( 0x18 ),	/* 24 */
/* 968 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 972 */	NdrFcShort( 0x38 ),	/* 56 */
/* 974 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x3,		/* 3 */
/* 976 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 978 */	NdrFcShort( 0x1 ),	/* 1 */
/* 980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 982 */	NdrFcShort( 0x0 ),	/* 0 */
/* 984 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter returned_results_count */

/* 986 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 988 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 990 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter returned_results_size */

/* 992 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 994 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 996 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter buffer */

/* 998 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1000 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1002 */	NdrFcShort( 0x15e ),	/* Type Offset=350 */

	/* Procedure kablam_command_get_ban_gamer_rpc */

/* 1004 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1006 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1010 */	NdrFcShort( 0x19 ),	/* 25 */
/* 1012 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1014 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1016 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1018 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x3,		/* 3 */
/* 1020 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1022 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1024 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1028 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter returned_results_count */

/* 1030 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1032 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1034 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter returned_results_size */

/* 1036 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1038 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1040 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter buffer */

/* 1042 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1044 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1046 */	NdrFcShort( 0x18c ),	/* Type Offset=396 */

	/* Procedure kablam_command_vip_add_rpc */

/* 1048 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1050 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1054 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1056 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1058 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1060 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1062 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x2,		/* 2 */
/* 1064 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1066 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1070 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1072 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter gamer_tag */

/* 1074 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1076 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1078 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter result_code */

/* 1080 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 1082 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1084 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_vip_remove_rpc */

/* 1086 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1088 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1092 */	NdrFcShort( 0x1b ),	/* 27 */
/* 1094 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1096 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1098 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1100 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x2,		/* 2 */
/* 1102 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1106 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1110 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter gamer_tag */

/* 1112 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1114 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1116 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter result_code */

/* 1118 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 1120 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1122 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_vip_clear_rpc */

/* 1124 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1126 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1130 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1132 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1136 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1138 */	0x40,		/* Oi2 Flags:  has ext, */
			0x1,		/* 1 */
/* 1140 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1148 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter arg_1 */

/* 1150 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 1152 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1154 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_get_vip_rpc */

/* 1156 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1158 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1162 */	NdrFcShort( 0x1d ),	/* 29 */
/* 1164 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1168 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1170 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x3,		/* 3 */
/* 1172 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1174 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1180 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter returned_results_count */

/* 1182 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1184 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1186 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter returned_results_size */

/* 1188 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1190 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1192 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter buffer */

/* 1194 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1196 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1198 */	NdrFcShort( 0x1be ),	/* Type Offset=446 */

	/* Procedure kablam_command_get_stats_folder_rpc */

/* 1200 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1202 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1206 */	NdrFcShort( 0x1e ),	/* 30 */
/* 1208 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1210 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1212 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1214 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x3,		/* 3 */
/* 1216 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1218 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1220 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1224 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter result_code */

/* 1226 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 1228 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1230 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter buffer_length */

/* 1232 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1234 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1236 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_path */

/* 1238 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1240 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1242 */	NdrFcShort( 0x1e0 ),	/* Type Offset=480 */

	/* Procedure kablam_command_set_stats_folder_rpc */

/* 1244 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1246 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1250 */	NdrFcShort( 0x1f ),	/* 31 */
/* 1252 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1256 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1258 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x2,		/* 2 */
/* 1260 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1268 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter in_path */

/* 1270 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1272 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1274 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Parameter result_code */

/* 1276 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 1278 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1280 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_set_privacy_rpc */

/* 1282 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1284 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1288 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1290 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1292 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1294 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1296 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 1298 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1302 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1306 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter privacy_type */

/* 1308 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1310 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1312 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter result_code */

/* 1314 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 1316 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1318 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_get_privacy_rpc */

/* 1320 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1322 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1326 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1328 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1332 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1334 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 1336 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1340 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1344 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter out_privacy */

/* 1346 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 1348 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1350 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter result_code */

/* 1352 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 1354 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1356 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_set_max_players_rpc */

/* 1358 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1360 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1364 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1366 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1368 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1370 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1372 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 1374 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1382 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter player_count */

/* 1384 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1386 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter result_code */

/* 1390 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 1392 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1394 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Procedure kablam_command_get_max_players_rpc */

/* 1396 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1398 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1402 */	NdrFcShort( 0x23 ),	/* 35 */
/* 1404 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1408 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1410 */	0x40,		/* Oi2 Flags:  has ext, */
			0x1,		/* 1 */
/* 1412 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1416 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1418 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1420 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter out_player_count */

/* 1422 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1424 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1426 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const kablam_commands_MIDL_TYPE_FORMAT_STRING kablam_commands__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	
			0x22,		/* FC_C_CSTRING */
			0x44,		/* FC_STRING_SIZED */
/*  8 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 10 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 12 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 14 */	
			0x11, 0x0,	/* FC_RP */
/* 16 */	NdrFcShort( 0x2 ),	/* Offset= 2 (18) */
/* 18 */	
			0x22,		/* FC_C_CSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 20 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 22 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 24 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 26 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 28 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 30 */	
			0x11, 0x0,	/* FC_RP */
/* 32 */	NdrFcShort( 0x8 ),	/* Offset= 8 (40) */
/* 34 */	
			0x1d,		/* FC_SMFARRAY */
			0x1,		/* 1 */
/* 36 */	NdrFcShort( 0x40 ),	/* 64 */
/* 38 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 40 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x1,		/* 1 */
/* 42 */	NdrFcShort( 0x104 ),	/* 260 */
/* 44 */	NdrFcShort( 0x0 ),	/* 0 */
/* 46 */	NdrFcShort( 0x0 ),	/* Offset= 0 (46) */
/* 48 */	0xd,		/* FC_ENUM16 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 50 */	0x0,		/* 0 */
			NdrFcShort( 0xffef ),	/* Offset= -17 (34) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 54 */	0x0,		/* 0 */
			NdrFcShort( 0xffeb ),	/* Offset= -21 (34) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 58 */	0x0,		/* 0 */
			NdrFcShort( 0xffe7 ),	/* Offset= -25 (34) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 62 */	0x0,		/* 0 */
			NdrFcShort( 0xffe3 ),	/* Offset= -29 (34) */
			0x5b,		/* FC_END */
/* 66 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 68 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 70 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 72 */	0xd,		/* FC_ENUM16 */
			0x5c,		/* FC_PAD */
/* 74 */	
			0x11, 0x0,	/* FC_RP */
/* 76 */	NdrFcShort( 0x1e ),	/* Offset= 30 (106) */
/* 78 */	
			0x1d,		/* FC_SMFARRAY */
			0x1,		/* 1 */
/* 80 */	NdrFcShort( 0x400 ),	/* 1024 */
/* 82 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 84 */	NdrFcShort( 0xffce ),	/* Offset= -50 (34) */
/* 86 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 88 */	
			0x1d,		/* FC_SMFARRAY */
			0x1,		/* 1 */
/* 90 */	NdrFcShort( 0x200 ),	/* 512 */
/* 92 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 94 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 96 */	NdrFcShort( 0x10 ),	/* 16 */
/* 98 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 100 */	
			0x1d,		/* FC_SMFARRAY */
			0x1,		/* 1 */
/* 102 */	NdrFcShort( 0x20 ),	/* 32 */
/* 104 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 106 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 108 */	NdrFcShort( 0xb88 ),	/* 2952 */
/* 110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x0 ),	/* Offset= 0 (112) */
/* 114 */	0xd,		/* FC_ENUM16 */
			0xe,		/* FC_ENUM32 */
/* 116 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 118 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 120 */	NdrFcShort( 0xffaa ),	/* Offset= -86 (34) */
/* 122 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 124 */	NdrFcShort( 0xffa6 ),	/* Offset= -90 (34) */
/* 126 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 128 */	NdrFcShort( 0xffa2 ),	/* Offset= -94 (34) */
/* 130 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 132 */	NdrFcShort( 0xff9e ),	/* Offset= -98 (34) */
/* 134 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 136 */	0x0,		/* 0 */
			NdrFcShort( 0xffc5 ),	/* Offset= -59 (78) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 140 */	0x0,		/* 0 */
			NdrFcShort( 0xffcb ),	/* Offset= -53 (88) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 144 */	0x0,		/* 0 */
			NdrFcShort( 0xffc7 ),	/* Offset= -57 (88) */
			0x3e,		/* FC_STRUCTPAD2 */
/* 148 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 150 */	0x0,		/* 0 */
			NdrFcShort( 0xffc1 ),	/* Offset= -63 (88) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 154 */	0x0,		/* 0 */
			NdrFcShort( 0xffc3 ),	/* Offset= -61 (94) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 158 */	0x0,		/* 0 */
			NdrFcShort( 0xff83 ),	/* Offset= -125 (34) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 162 */	0x0,		/* 0 */
			NdrFcShort( 0xffc1 ),	/* Offset= -63 (100) */
			0x5b,		/* FC_END */
/* 166 */	
			0x11, 0x0,	/* FC_RP */
/* 168 */	NdrFcShort( 0x2 ),	/* Offset= 2 (170) */
/* 170 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 172 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 174 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 176 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 178 */	
			0x11, 0x0,	/* FC_RP */
/* 180 */	NdrFcShort( 0x2 ),	/* Offset= 2 (182) */
/* 182 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 184 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 186 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 188 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 190 */	
			0x11, 0x0,	/* FC_RP */
/* 192 */	NdrFcShort( 0x2 ),	/* Offset= 2 (194) */
/* 194 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 196 */	0x40,		/* Corr desc:  constant, val=122 */
			0x0,		/* 0 */
/* 198 */	NdrFcShort( 0x7a ),	/* 122 */
/* 200 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 202 */	
			0x11, 0x0,	/* FC_RP */
/* 204 */	NdrFcShort( 0x30 ),	/* Offset= 48 (252) */
/* 206 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 208 */	NdrFcShort( 0xc8 ),	/* 200 */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x0 ),	/* Offset= 0 (212) */
/* 214 */	0xd,		/* FC_ENUM16 */
			0x8,		/* FC_LONG */
/* 216 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 218 */	NdrFcShort( 0xff48 ),	/* Offset= -184 (34) */
/* 220 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 222 */	NdrFcShort( 0xff44 ),	/* Offset= -188 (34) */
/* 224 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 226 */	NdrFcShort( 0xff40 ),	/* Offset= -192 (34) */
/* 228 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 230 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 232 */	NdrFcShort( 0xc8 ),	/* 200 */
/* 234 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 238 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 240 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 244 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 246 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 248 */	NdrFcShort( 0xffd6 ),	/* Offset= -42 (206) */
/* 250 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 252 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 254 */	NdrFcShort( 0x9e50 ),	/* -25008 */
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x0 ),	/* Offset= 0 (258) */
/* 260 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 262 */	0xd,		/* FC_ENUM16 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 264 */	0x0,		/* 0 */
			NdrFcShort( 0xff4f ),	/* Offset= -177 (88) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 268 */	0x0,		/* 0 */
			NdrFcShort( 0xffd9 ),	/* Offset= -39 (230) */
			0x8,		/* FC_LONG */
/* 272 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 274 */	
			0x11, 0x0,	/* FC_RP */
/* 276 */	NdrFcShort( 0x8 ),	/* Offset= 8 (284) */
/* 278 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 280 */	NdrFcShort( 0x6 ),	/* 6 */
/* 282 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 284 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 286 */	NdrFcShort( 0x6 ),	/* 6 */
/* 288 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 290 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (278) */
/* 292 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 294 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 296 */	NdrFcShort( 0x2 ),	/* Offset= 2 (298) */
/* 298 */	
			0x12, 0x20,	/* FC_UP [maybenull_sizeis] */
/* 300 */	NdrFcShort( 0x1c ),	/* Offset= 28 (328) */
/* 302 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 304 */	NdrFcShort( 0x8 ),	/* 8 */
/* 306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x0 ),	/* Offset= 0 (308) */
/* 310 */	0x8,		/* FC_LONG */
			0x2,		/* FC_CHAR */
/* 312 */	0x3f,		/* FC_STRUCTPAD3 */
			0x5b,		/* FC_END */
/* 314 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 316 */	NdrFcShort( 0x10 ),	/* 16 */
/* 318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 320 */	NdrFcShort( 0x0 ),	/* Offset= 0 (320) */
/* 322 */	0xb,		/* FC_HYPER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 324 */	0x0,		/* 0 */
			NdrFcShort( 0xffe9 ),	/* Offset= -23 (302) */
			0x5b,		/* FC_END */
/* 328 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x54,		/* FC_DEREFERENCE */
/* 334 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 336 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 338 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 342 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 344 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 346 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (314) */
/* 348 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 350 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 352 */	NdrFcShort( 0x2 ),	/* Offset= 2 (354) */
/* 354 */	
			0x12, 0x20,	/* FC_UP [maybenull_sizeis] */
/* 356 */	NdrFcShort( 0x12 ),	/* Offset= 18 (374) */
/* 358 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 360 */	NdrFcShort( 0x10 ),	/* 16 */
/* 362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0x0 ),	/* Offset= 0 (364) */
/* 366 */	0xb,		/* FC_HYPER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 368 */	0x0,		/* 0 */
			NdrFcShort( 0xffab ),	/* Offset= -85 (284) */
			0x3e,		/* FC_STRUCTPAD2 */
/* 372 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 374 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 378 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x54,		/* FC_DEREFERENCE */
/* 380 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 382 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 384 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 388 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 390 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 392 */	NdrFcShort( 0xffde ),	/* Offset= -34 (358) */
/* 394 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 396 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 398 */	NdrFcShort( 0x2 ),	/* Offset= 2 (400) */
/* 400 */	
			0x12, 0x20,	/* FC_UP [maybenull_sizeis] */
/* 402 */	NdrFcShort( 0x16 ),	/* Offset= 22 (424) */
/* 404 */	
			0x15,		/* FC_STRUCT */
			0x1,		/* 1 */
/* 406 */	NdrFcShort( 0x20 ),	/* 32 */
/* 408 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 410 */	NdrFcShort( 0xfeca ),	/* Offset= -310 (100) */
/* 412 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 414 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 416 */	NdrFcShort( 0x28 ),	/* 40 */
/* 418 */	0xb,		/* FC_HYPER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 420 */	0x0,		/* 0 */
			NdrFcShort( 0xffef ),	/* Offset= -17 (404) */
			0x5b,		/* FC_END */
/* 424 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 428 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x54,		/* FC_DEREFERENCE */
/* 430 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 432 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 434 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 438 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 440 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 442 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (414) */
/* 444 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 446 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 448 */	NdrFcShort( 0x2 ),	/* Offset= 2 (450) */
/* 450 */	
			0x12, 0x20,	/* FC_UP [maybenull_sizeis] */
/* 452 */	NdrFcShort( 0x2 ),	/* Offset= 2 (454) */
/* 454 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x1,		/* 1 */
/* 456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 458 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x54,		/* FC_DEREFERENCE */
/* 460 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 462 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 464 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 468 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 470 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 472 */	NdrFcShort( 0xffbc ),	/* Offset= -68 (404) */
/* 474 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 476 */	
			0x11, 0x0,	/* FC_RP */
/* 478 */	NdrFcShort( 0x2 ),	/* Offset= 2 (480) */
/* 480 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 482 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 484 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 486 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */

			0x0
        }
    };

static const unsigned short kablam_commands_interface_FormatStringOffsetTable[] =
    {
    0,
    56,
    82,
    114,
    152,
    184,
    228,
    266,
    310,
    348,
    386,
    424,
    456,
    494,
    544,
    588,
    620,
    670,
    714,
    758,
    802,
    840,
    878,
    916,
    960,
    1004,
    1048,
    1086,
    1124,
    1156,
    1200,
    1244,
    1282,
    1320,
    1358,
    1396
    };



#endif /* defined(_M_AMD64)*/



/* this ALWAYS GENERATED file contains the RPC client stubs */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for kablam_rpc\kablam_commands.idl:
    Oicf, W0, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, app_config, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)




#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif


#include "ndr64types.h"
#include "pshpack8.h"
#ifdef __cplusplus
namespace {
#endif


typedef 
NDR64_FORMAT_CHAR
__midl_frag217_t;
extern const __midl_frag217_t __midl_frag217;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag216_t;
extern const __midl_frag216_t __midl_frag216;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
}
__midl_frag215_t;
extern const __midl_frag215_t __midl_frag215;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag211_t;
extern const __midl_frag211_t __midl_frag211;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag206_t;
extern const __midl_frag206_t __midl_frag206;

typedef 
struct _NDR64_CONFORMANT_STRING_FORMAT
__midl_frag199_t;
extern const __midl_frag199_t __midl_frag199;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag198_t;
extern const __midl_frag198_t __midl_frag198;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag197_t;
extern const __midl_frag197_t __midl_frag197;

typedef 
struct NDR64_SIZED_CONFORMANT_STRING_FORMAT
__midl_frag196_t;
extern const __midl_frag196_t __midl_frag196;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag195_t;
extern const __midl_frag195_t __midl_frag195;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag194_t;
extern const __midl_frag194_t __midl_frag194;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag190_t;
extern const __midl_frag190_t __midl_frag190;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag189_t;
extern const __midl_frag189_t __midl_frag189;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_OPERATOR frag2;
    struct _NDR64_EXPR_VAR frag3;
}
__midl_frag188_t;
extern const __midl_frag188_t __midl_frag188;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag187_t;
extern const __midl_frag187_t __midl_frag187;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag186_t;
extern const __midl_frag186_t __midl_frag186;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag185_t;
extern const __midl_frag185_t __midl_frag185;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag180_t;
extern const __midl_frag180_t __midl_frag180;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
}
__midl_frag177_t;
extern const __midl_frag177_t __midl_frag177;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag172_t;
extern const __midl_frag172_t __midl_frag172;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag166_t;
extern const __midl_frag166_t __midl_frag166;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_OPERATOR frag2;
    struct _NDR64_EXPR_VAR frag3;
}
__midl_frag165_t;
extern const __midl_frag165_t __midl_frag165;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag164_t;
extern const __midl_frag164_t __midl_frag164;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag163_t;
extern const __midl_frag163_t __midl_frag163;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag162_t;
extern const __midl_frag162_t __midl_frag162;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag157_t;
extern const __midl_frag157_t __midl_frag157;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag156_t;
extern const __midl_frag156_t __midl_frag156;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_OPERATOR frag2;
    struct _NDR64_EXPR_VAR frag3;
}
__midl_frag155_t;
extern const __midl_frag155_t __midl_frag155;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag154_t;
extern const __midl_frag154_t __midl_frag154;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag153_t;
extern const __midl_frag153_t __midl_frag153;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag152_t;
extern const __midl_frag152_t __midl_frag152;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag147_t;
extern const __midl_frag147_t __midl_frag147;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag146_t;
extern const __midl_frag146_t __midl_frag146;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_OPERATOR frag2;
    struct _NDR64_EXPR_VAR frag3;
}
__midl_frag145_t;
extern const __midl_frag145_t __midl_frag145;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag144_t;
extern const __midl_frag144_t __midl_frag144;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag143_t;
extern const __midl_frag143_t __midl_frag143;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag142_t;
extern const __midl_frag142_t __midl_frag142;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag137_t;
extern const __midl_frag137_t __midl_frag137;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag133_t;
extern const __midl_frag133_t __midl_frag133;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag128_t;
extern const __midl_frag128_t __midl_frag128;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag125_t;
extern const __midl_frag125_t __midl_frag125;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag124_t;
extern const __midl_frag124_t __midl_frag124;

typedef 
NDR64_FORMAT_CHAR
__midl_frag121_t;
extern const __midl_frag121_t __midl_frag121;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag119_t;
extern const __midl_frag119_t __midl_frag119;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag113_t;
extern const __midl_frag113_t __midl_frag113;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag109_t;
extern const __midl_frag109_t __midl_frag109;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag107_t;
extern const __midl_frag107_t __midl_frag107;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag101_t;
extern const __midl_frag101_t __midl_frag101;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
}
__midl_frag98_t;
extern const __midl_frag98_t __midl_frag98;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag91_t;
extern const __midl_frag91_t __midl_frag91;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag82_t;
extern const __midl_frag82_t __midl_frag82;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag76_t;
extern const __midl_frag76_t __midl_frag76;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
}
__midl_frag75_t;
extern const __midl_frag75_t __midl_frag75;

typedef 
struct 
{
    struct _NDR64_FIX_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag74_t;
extern const __midl_frag74_t __midl_frag74;

typedef 
struct 
{
    struct _NDR64_FIX_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag73_t;
extern const __midl_frag73_t __midl_frag73;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag4;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag5;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag6;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag7;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag8;
    } frag2;
}
__midl_frag72_t;
extern const __midl_frag72_t __midl_frag72;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag68_t;
extern const __midl_frag68_t __midl_frag68;

typedef 
struct NDR64_SIZED_CONFORMANT_STRING_FORMAT
__midl_frag65_t;
extern const __midl_frag65_t __midl_frag65;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_NOOP frag2;
    struct _NDR64_EXPR_CONST64 frag3;
}
__midl_frag64_t;
extern const __midl_frag64_t __midl_frag64;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag63_t;
extern const __midl_frag63_t __midl_frag63;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag62_t;
extern const __midl_frag62_t __midl_frag62;

typedef 
struct NDR64_SIZED_CONFORMANT_STRING_FORMAT
__midl_frag56_t;
extern const __midl_frag56_t __midl_frag56;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag55_t;
extern const __midl_frag55_t __midl_frag55;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag54_t;
extern const __midl_frag54_t __midl_frag54;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag50_t;
extern const __midl_frag50_t __midl_frag50;

typedef 
struct 
{
    struct _NDR64_FIX_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag37_t;
extern const __midl_frag37_t __midl_frag37;

typedef 
struct 
{
    struct _NDR64_FIX_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag36_t;
extern const __midl_frag36_t __midl_frag36;

typedef 
struct 
{
    struct _NDR64_FIX_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag35_t;
extern const __midl_frag35_t __midl_frag35;

typedef 
NDR64_FORMAT_CHAR
__midl_frag31_t;
extern const __midl_frag31_t __midl_frag31;

typedef 
struct 
{
    struct _NDR64_BOGUS_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag30_t;
extern const __midl_frag30_t __midl_frag30;

typedef 
struct 
{
    struct _NDR64_BOGUS_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag29_t;
extern const __midl_frag29_t __midl_frag29;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag5;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag6;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag7;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag8;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag9;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag10;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag11;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag12;
        struct _NDR64_MEMPAD_FORMAT frag13;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag14;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag15;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag16;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag17;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag18;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag19;
    } frag2;
}
__midl_frag24_t;
extern const __midl_frag24_t __midl_frag24;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag23_t;
extern const __midl_frag23_t __midl_frag23;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
}
__midl_frag22_t;
extern const __midl_frag22_t __midl_frag22;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag16_t;
extern const __midl_frag16_t __midl_frag16;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag15_t;
extern const __midl_frag15_t __midl_frag15;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
}
__midl_frag14_t;
extern const __midl_frag14_t __midl_frag14;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
}
__midl_frag13_t;
extern const __midl_frag13_t __midl_frag13;

typedef 
struct NDR64_SIZED_CONFORMANT_STRING_FORMAT
__midl_frag10_t;
extern const __midl_frag10_t __midl_frag10;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag9_t;
extern const __midl_frag9_t __midl_frag9;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag8_t;
extern const __midl_frag8_t __midl_frag8;

typedef 
struct NDR64_SIZED_CONFORMANT_STRING_FORMAT
__midl_frag6_t;
extern const __midl_frag6_t __midl_frag6;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag5_t;
extern const __midl_frag5_t __midl_frag5;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag4_t;
extern const __midl_frag4_t __midl_frag4;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag2_t;
extern const __midl_frag2_t __midl_frag2;

typedef 
NDR64_FORMAT_UINT32
__midl_frag1_t;
extern const __midl_frag1_t __midl_frag1;

static const __midl_frag217_t __midl_frag217 =
0x5    /* FC64_INT32 */;

static const __midl_frag216_t __midl_frag216 =
{ 
/* *long */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 12 /* 0xc */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag217
};

static const __midl_frag215_t __midl_frag215 =
{ 
/* kablam_command_get_max_players_rpc */
    { 
    /* kablam_command_get_max_players_rpc */      /* procedure kablam_command_get_max_players_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 8 /* 0x8 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 1 /* 0x1 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* out_player_count */      /* parameter out_player_count */
        &__midl_frag217,
        { 
        /* out_player_count */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    }
};

static const __midl_frag211_t __midl_frag211 =
{ 
/* kablam_command_set_max_players_rpc */
    { 
    /* kablam_command_set_max_players_rpc */      /* procedure kablam_command_set_max_players_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 16 /* 0x10 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* player_count */      /* parameter player_count */
        &__midl_frag217,
        { 
        /* player_count */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    }
};

static const __midl_frag206_t __midl_frag206 =
{ 
/* kablam_command_get_privacy_rpc */
    { 
    /* kablam_command_get_privacy_rpc */      /* procedure kablam_command_get_privacy_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 16 /* 0x10 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 64 /* 0x40 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* out_privacy */      /* parameter out_privacy */
        &__midl_frag217,
        { 
        /* out_privacy */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    }
};

static const __midl_frag199_t __midl_frag199 =
{ 
/* *wchar_t */
    { 
    /* *wchar_t */
        0x64,    /* FC64_CONF_WCHAR_STRING */
        { 
        /* *wchar_t */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT16) 2 /* 0x2 */
    }
};

static const __midl_frag198_t __midl_frag198 =
{ 
/* *wchar_t */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag199
};

static const __midl_frag197_t __midl_frag197 =
{ 
/* kablam_command_set_stats_folder_rpc */
    { 
    /* kablam_command_set_stats_folder_rpc */      /* procedure kablam_command_set_stats_folder_rpc */
        (NDR64_UINT32) 262210 /* 0x40042 */,    /* primitive handle */ /* IsIntrepreted, ClientMustSize */
        (NDR64_UINT32) 16 /* 0x10 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* in_path */      /* parameter in_path */
        &__midl_frag199,
        { 
        /* in_path */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    }
};

static const __midl_frag196_t __midl_frag196 =
{ 
/* *wchar_t */
    { 
    /* *wchar_t */
        0x64,    /* FC64_CONF_WCHAR_STRING */
        { 
        /* *wchar_t */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT16) 2 /* 0x2 */
    },
    &__midl_frag195
};

static const __midl_frag195_t __midl_frag195 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x5,    /* FC64_INT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */  /* Offset */
    }
};

static const __midl_frag194_t __midl_frag194 =
{ 
/* *wchar_t */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag196
};

static const __midl_frag190_t __midl_frag190 =
{ 
/* kablam_command_get_stats_folder_rpc */
    { 
    /* kablam_command_get_stats_folder_rpc */      /* procedure kablam_command_get_stats_folder_rpc */
        (NDR64_UINT32) 4325442 /* 0x420042 */,    /* primitive handle */ /* IsIntrepreted, ServerMustSize, ClientCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* buffer_length */      /* parameter buffer_length */
        &__midl_frag217,
        { 
        /* buffer_length */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* out_path */      /* parameter out_path */
        &__midl_frag196,
        { 
        /* out_path */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag189_t __midl_frag189 =
{ 
/* s_gamertag */
    { 
    /* s_gamertag */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* s_gamertag */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */
    }
};

static const __midl_frag188_t __midl_frag188 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_OPERATOR */
        0x4,    /* FC_EXPR_OPER */
        0x5,    /* OP_UNARY_INDIRECTION */
        0x5,    /* FC64_INT32 */
        (NDR64_UINT8) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x7,    /* FC64_INT64 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */  /* Offset */
    }
};

static const __midl_frag187_t __midl_frag187 =
{ 
/* *s_gamertag */
    { 
    /* *s_gamertag */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* *s_gamertag */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        &__midl_frag188
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 32 /* 0x20 */,
        &__midl_frag189
    }
};

static const __midl_frag186_t __midl_frag186 =
{ 
/* *s_gamertag */
    0x21,    /* FC64_UP */
    (NDR64_UINT8) 32 /* 0x20 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag187
};

static const __midl_frag185_t __midl_frag185 =
{ 
/* **s_gamertag */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 20 /* 0x14 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag186
};

static const __midl_frag180_t __midl_frag180 =
{ 
/* kablam_command_get_vip_rpc */
    { 
    /* kablam_command_get_vip_rpc */      /* procedure kablam_command_get_vip_rpc */
        (NDR64_UINT32) 4325442 /* 0x420042 */,    /* primitive handle */ /* IsIntrepreted, ServerMustSize, ClientCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 64 /* 0x40 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* returned_results_count */      /* parameter returned_results_count */
        &__midl_frag217,
        { 
        /* returned_results_count */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* returned_results_size */      /* parameter returned_results_size */
        &__midl_frag217,
        { 
        /* returned_results_size */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* buffer */      /* parameter buffer */
        &__midl_frag185,
        { 
        /* buffer */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustSize, MustFree, [out], UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag177_t __midl_frag177 =
{ 
/* kablam_command_vip_clear_rpc */
    { 
    /* kablam_command_vip_clear_rpc */      /* procedure kablam_command_vip_clear_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 8 /* 0x8 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 1 /* 0x1 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* arg_1 */      /* parameter arg_1 */
        &__midl_frag217,
        { 
        /* arg_1 */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    }
};

static const __midl_frag172_t __midl_frag172 =
{ 
/* kablam_command_vip_remove_rpc */
    { 
    /* kablam_command_vip_remove_rpc */      /* procedure kablam_command_vip_remove_rpc */
        (NDR64_UINT32) 262210 /* 0x40042 */,    /* primitive handle */ /* IsIntrepreted, ClientMustSize */
        (NDR64_UINT32) 16 /* 0x10 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* gamer_tag */      /* parameter gamer_tag */
        &__midl_frag199,
        { 
        /* gamer_tag */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    }
};

static const __midl_frag166_t __midl_frag166 =
{ 
/* s_expiration_entry_gamer */
    { 
    /* s_expiration_entry_gamer */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* s_expiration_entry_gamer */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 40 /* 0x28 */
    }
};

static const __midl_frag165_t __midl_frag165 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_OPERATOR */
        0x4,    /* FC_EXPR_OPER */
        0x5,    /* OP_UNARY_INDIRECTION */
        0x5,    /* FC64_INT32 */
        (NDR64_UINT8) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x7,    /* FC64_INT64 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */  /* Offset */
    }
};

static const __midl_frag164_t __midl_frag164 =
{ 
/* *s_expiration_entry_gamer */
    { 
    /* *s_expiration_entry_gamer */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* *s_expiration_entry_gamer */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        &__midl_frag165
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 40 /* 0x28 */,
        &__midl_frag166
    }
};

static const __midl_frag163_t __midl_frag163 =
{ 
/* *s_expiration_entry_gamer */
    0x21,    /* FC64_UP */
    (NDR64_UINT8) 32 /* 0x20 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag164
};

static const __midl_frag162_t __midl_frag162 =
{ 
/* **s_expiration_entry_gamer */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 20 /* 0x14 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag163
};

static const __midl_frag157_t __midl_frag157 =
{ 
/* kablam_command_get_ban_gamer_rpc */
    { 
    /* kablam_command_get_ban_gamer_rpc */      /* procedure kablam_command_get_ban_gamer_rpc */
        (NDR64_UINT32) 4325442 /* 0x420042 */,    /* primitive handle */ /* IsIntrepreted, ServerMustSize, ClientCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 64 /* 0x40 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* returned_results_count */      /* parameter returned_results_count */
        &__midl_frag217,
        { 
        /* returned_results_count */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* returned_results_size */      /* parameter returned_results_size */
        &__midl_frag217,
        { 
        /* returned_results_size */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* buffer */      /* parameter buffer */
        &__midl_frag162,
        { 
        /* buffer */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustSize, MustFree, [out], UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag156_t __midl_frag156 =
{ 
/* s_expiration_entry_mac_address */
    { 
    /* s_expiration_entry_mac_address */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* s_expiration_entry_mac_address */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */
    }
};

static const __midl_frag155_t __midl_frag155 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_OPERATOR */
        0x4,    /* FC_EXPR_OPER */
        0x5,    /* OP_UNARY_INDIRECTION */
        0x5,    /* FC64_INT32 */
        (NDR64_UINT8) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x7,    /* FC64_INT64 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */  /* Offset */
    }
};

static const __midl_frag154_t __midl_frag154 =
{ 
/* *s_expiration_entry_mac_address */
    { 
    /* *s_expiration_entry_mac_address */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* *s_expiration_entry_mac_address */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        &__midl_frag155
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 16 /* 0x10 */,
        &__midl_frag156
    }
};

static const __midl_frag153_t __midl_frag153 =
{ 
/* *s_expiration_entry_mac_address */
    0x21,    /* FC64_UP */
    (NDR64_UINT8) 32 /* 0x20 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag154
};

static const __midl_frag152_t __midl_frag152 =
{ 
/* **s_expiration_entry_mac_address */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 20 /* 0x14 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag153
};

static const __midl_frag147_t __midl_frag147 =
{ 
/* kablam_command_get_ban_nic_rpc */
    { 
    /* kablam_command_get_ban_nic_rpc */      /* procedure kablam_command_get_ban_nic_rpc */
        (NDR64_UINT32) 4325442 /* 0x420042 */,    /* primitive handle */ /* IsIntrepreted, ServerMustSize, ClientCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 64 /* 0x40 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* returned_results_count */      /* parameter returned_results_count */
        &__midl_frag217,
        { 
        /* returned_results_count */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* returned_results_size */      /* parameter returned_results_size */
        &__midl_frag217,
        { 
        /* returned_results_size */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* buffer */      /* parameter buffer */
        &__midl_frag152,
        { 
        /* buffer */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustSize, MustFree, [out], UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag146_t __midl_frag146 =
{ 
/* s_exipration_entry_ipv4_subnet */
    { 
    /* s_exipration_entry_ipv4_subnet */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* s_exipration_entry_ipv4_subnet */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */
    }
};

static const __midl_frag145_t __midl_frag145 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_OPERATOR */
        0x4,    /* FC_EXPR_OPER */
        0x5,    /* OP_UNARY_INDIRECTION */
        0x5,    /* FC64_INT32 */
        (NDR64_UINT8) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x7,    /* FC64_INT64 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */  /* Offset */
    }
};

static const __midl_frag144_t __midl_frag144 =
{ 
/* *s_exipration_entry_ipv4_subnet */
    { 
    /* *s_exipration_entry_ipv4_subnet */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* *s_exipration_entry_ipv4_subnet */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        &__midl_frag145
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 16 /* 0x10 */,
        &__midl_frag146
    }
};

static const __midl_frag143_t __midl_frag143 =
{ 
/* *s_exipration_entry_ipv4_subnet */
    0x21,    /* FC64_UP */
    (NDR64_UINT8) 32 /* 0x20 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag144
};

static const __midl_frag142_t __midl_frag142 =
{ 
/* **s_exipration_entry_ipv4_subnet */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 20 /* 0x14 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag143
};

static const __midl_frag137_t __midl_frag137 =
{ 
/* kablam_command_get_ban_ip_rpc */
    { 
    /* kablam_command_get_ban_ip_rpc */      /* procedure kablam_command_get_ban_ip_rpc */
        (NDR64_UINT32) 4325442 /* 0x420042 */,    /* primitive handle */ /* IsIntrepreted, ServerMustSize, ClientCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 64 /* 0x40 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* returned_results_count */      /* parameter returned_results_count */
        &__midl_frag217,
        { 
        /* returned_results_count */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* returned_results_size */      /* parameter returned_results_size */
        &__midl_frag217,
        { 
        /* returned_results_size */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* buffer */      /* parameter buffer */
        &__midl_frag142,
        { 
        /* buffer */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustSize, MustFree, [out], UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag133_t __midl_frag133 =
{ 
/* kablam_command_unban_all_rpc */
    { 
    /* kablam_command_unban_all_rpc */      /* procedure kablam_command_unban_all_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 16 /* 0x10 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* unban_all_type */      /* parameter unban_all_type */
        &__midl_frag217,
        { 
        /* unban_all_type */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    }
};

static const __midl_frag128_t __midl_frag128 =
{ 
/* kablam_command_unban_gamer_rpc */
    { 
    /* kablam_command_unban_gamer_rpc */      /* procedure kablam_command_unban_gamer_rpc */
        (NDR64_UINT32) 262210 /* 0x40042 */,    /* primitive handle */ /* IsIntrepreted, ClientMustSize */
        (NDR64_UINT32) 16 /* 0x10 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* gamer_tag */      /* parameter gamer_tag */
        &__midl_frag199,
        { 
        /* gamer_tag */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    }
};

static const __midl_frag125_t __midl_frag125 =
{ 
/* *s_mac_address */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag109
};

static const __midl_frag124_t __midl_frag124 =
{ 
/* kablam_command_unban_nic_rpc */
    { 
    /* kablam_command_unban_nic_rpc */      /* procedure kablam_command_unban_nic_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 16 /* 0x10 */ ,  /* Stack size */
        (NDR64_UINT32) 62 /* 0x3e */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* mac_address */      /* parameter mac_address */
        &__midl_frag109,
        { 
        /* mac_address */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    }
};

static const __midl_frag121_t __midl_frag121 =
0x10    /* FC64_CHAR */;

static const __midl_frag119_t __midl_frag119 =
{ 
/* kablam_command_unban_ip_rpc */
    { 
    /* kablam_command_unban_ip_rpc */      /* procedure kablam_command_unban_ip_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 13 /* 0xd */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* ipv4_address */      /* parameter ipv4_address */
        &__midl_frag217,
        { 
        /* ipv4_address */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* cidr */      /* parameter cidr */
        &__midl_frag121,
        { 
        /* cidr */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag113_t __midl_frag113 =
{ 
/* kablam_command_ban_gamer_rpc */
    { 
    /* kablam_command_ban_gamer_rpc */      /* procedure kablam_command_ban_gamer_rpc */
        (NDR64_UINT32) 262210 /* 0x40042 */,    /* primitive handle */ /* IsIntrepreted, ClientMustSize */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* gamer_tag */      /* parameter gamer_tag */
        &__midl_frag199,
        { 
        /* gamer_tag */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* duration */      /* parameter duration */
        &__midl_frag217,
        { 
        /* duration */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag109_t __midl_frag109 =
{ 
/* s_mac_address */
    { 
    /* s_mac_address */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* s_mac_address */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 6 /* 0x6 */
    }
};

static const __midl_frag107_t __midl_frag107 =
{ 
/* kablam_command_ban_nic_rpc */
    { 
    /* kablam_command_ban_nic_rpc */      /* procedure kablam_command_ban_nic_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 70 /* 0x46 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* mac_address */      /* parameter mac_address */
        &__midl_frag109,
        { 
        /* mac_address */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* duration */      /* parameter duration */
        &__midl_frag217,
        { 
        /* duration */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag101_t __midl_frag101 =
{ 
/* kablam_command_ban_ip_rpc */
    { 
    /* kablam_command_ban_ip_rpc */      /* procedure kablam_command_ban_ip_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 21 /* 0x15 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* ipv4_address */      /* parameter ipv4_address */
        &__midl_frag217,
        { 
        /* ipv4_address */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* subnet */      /* parameter subnet */
        &__midl_frag121,
        { 
        /* subnet */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* duration */      /* parameter duration */
        &__midl_frag217,
        { 
        /* duration */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag98_t __midl_frag98 =
{ 
/* kablam_command_live_signout_rpc */
    { 
    /* kablam_command_live_signout_rpc */      /* procedure kablam_command_live_signout_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 8 /* 0x8 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 1 /* 0x1 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    }
};

static const __midl_frag91_t __midl_frag91 =
{ 
/* kablam_command_live_signin_rpc */
    { 
    /* kablam_command_live_signin_rpc */      /* procedure kablam_command_live_signin_rpc */
        (NDR64_UINT32) 262210 /* 0x40042 */,    /* primitive handle */ /* IsIntrepreted, ClientMustSize */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* username */      /* parameter username */
        &__midl_frag199,
        { 
        /* username */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* password */      /* parameter password */
        &__midl_frag199,
        { 
        /* password */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag82_t __midl_frag82 =
{ 
/* kablam_command_live_auto_signin_rpc */
    { 
    /* kablam_command_live_auto_signin_rpc */      /* procedure kablam_command_live_auto_signin_rpc */
        (NDR64_UINT32) 262210 /* 0x40042 */,    /* primitive handle */ /* IsIntrepreted, ClientMustSize */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 64 /* 0x40 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* username */      /* parameter username */
        &__midl_frag199,
        { 
        /* username */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* password */      /* parameter password */
        &__midl_frag199,
        { 
        /* password */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* xlive_login_result */      /* parameter xlive_login_result */
        &__midl_frag217,
        { 
        /* xlive_login_result */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag76_t __midl_frag76 =
{ 
/* *kablam_command_play_result */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag72
};

static const __midl_frag75_t __midl_frag75 =
{ 
/* kablam_command_playing_rpc */
    { 
    /* kablam_command_playing_rpc */      /* procedure kablam_command_playing_rpc */
        (NDR64_UINT32) 131138 /* 0x20042 */,    /* primitive handle */ /* IsIntrepreted, ServerMustSize */
        (NDR64_UINT32) 8 /* 0x8 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 1 /* 0x1 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* out_response */      /* parameter out_response */
        &__midl_frag72,
        { 
        /* out_response */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    }
};

static const __midl_frag74_t __midl_frag74 =
{ 
/*  */
    { 
    /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
        0x40,    /* FC64_FIX_ARRAY */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 40000 /* 0x9c40 */
    }
};

static const __midl_frag73_t __midl_frag73 =
{ 
/*  */
    { 
    /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
        0x40,    /* FC64_FIX_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 512 /* 0x200 */
    }
};

static const __midl_frag72_t __midl_frag72 =
{ 
/* kablam_command_play_result */
    { 
    /* kablam_command_play_result */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* kablam_command_play_result */
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 40528 /* 0x9e50 */,
        0,
        0,
        0,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x4,    /* FC64_INT16 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 2 /* 0x2 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag73
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag74
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag68_t __midl_frag68 =
{ 
/* kablam_command_play_rpc */
    { 
    /* kablam_command_play_rpc */      /* procedure kablam_command_play_rpc */
        (NDR64_UINT32) 393282 /* 0x60042 */,    /* primitive handle */ /* IsIntrepreted, ServerMustSize, ClientMustSize */
        (NDR64_UINT32) 16 /* 0x10 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* file_path */      /* parameter file_path */
        &__midl_frag199,
        { 
        /* file_path */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* out_response */      /* parameter out_response */
        &__midl_frag72,
        { 
        /* out_response */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    }
};

static const __midl_frag65_t __midl_frag65 =
{ 
/* *wchar_t */
    { 
    /* *wchar_t */
        0x64,    /* FC64_CONF_WCHAR_STRING */
        { 
        /* *wchar_t */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT16) 2 /* 0x2 */
    },
    &__midl_frag64
};

static const __midl_frag64_t __midl_frag64 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_NOOP */
        0x5,    /* FC_EXPR_PAD */
        (NDR64_UINT8) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_EXPR_CONST64 */
        0x2,    /* FC_EXPR_CONST64 */
        0x7,    /* FC64_INT64 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT64) 122 /* 0x7a */
    }
};

static const __midl_frag63_t __midl_frag63 =
{ 
/* *wchar_t */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag65
};

static const __midl_frag62_t __midl_frag62 =
{ 
/* kablam_command_send_message_rpc */
    { 
    /* kablam_command_send_message_rpc */      /* procedure kablam_command_send_message_rpc */
        (NDR64_UINT32) 2359362 /* 0x240042 */,    /* primitive handle */ /* IsIntrepreted, ClientMustSize, ServerCorrelation */
        (NDR64_UINT32) 16 /* 0x10 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* message */      /* parameter message */
        &__midl_frag65,
        { 
        /* message */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    }
};

static const __midl_frag56_t __midl_frag56 =
{ 
/* *wchar_t */
    { 
    /* *wchar_t */
        0x64,    /* FC64_CONF_WCHAR_STRING */
        { 
        /* *wchar_t */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT16) 2 /* 0x2 */
    },
    &__midl_frag55
};

static const __midl_frag55_t __midl_frag55 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x5,    /* FC64_INT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */  /* Offset */
    }
};

static const __midl_frag54_t __midl_frag54 =
{ 
/* *wchar_t */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag56
};

static const __midl_frag50_t __midl_frag50 =
{ 
/* kablam_command_get_description_rpc */
    { 
    /* kablam_command_get_description_rpc */      /* procedure kablam_command_get_description_rpc */
        (NDR64_UINT32) 4325442 /* 0x420042 */,    /* primitive handle */ /* IsIntrepreted, ServerMustSize, ClientCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* result_code */      /* parameter result_code */
        &__midl_frag217,
        { 
        /* result_code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* buffer_length */      /* parameter buffer_length */
        &__midl_frag217,
        { 
        /* buffer_length */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* out_description */      /* parameter out_description */
        &__midl_frag56,
        { 
        /* out_description */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag37_t __midl_frag37 =
{ 
/*  */
    { 
    /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
        0x40,    /* FC64_FIX_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */
    }
};

static const __midl_frag36_t __midl_frag36 =
{ 
/*  */
    { 
    /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
        0x40,    /* FC64_FIX_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 64 /* 0x40 */
    }
};

static const __midl_frag35_t __midl_frag35 =
{ 
/*  */
    { 
    /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
        0x40,    /* FC64_FIX_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */
    }
};

static const __midl_frag31_t __midl_frag31 =
0x11    /* FC64_WCHAR */;

static const __midl_frag30_t __midl_frag30 =
{ 
/*  */
    { 
    /* struct _NDR64_BOGUS_ARRAY_HEADER_FORMAT */
        0x44,    /* FC64_FIX_FORCED_BOGUS_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* struct _NDR64_BOGUS_ARRAY_HEADER_FORMAT */
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 1 /* 0x1 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        &__midl_frag31
    }
};

static const __midl_frag29_t __midl_frag29 =
{ 
/*  */
    { 
    /* struct _NDR64_BOGUS_ARRAY_HEADER_FORMAT */
        0x44,    /* FC64_FIX_FORCED_BOGUS_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* struct _NDR64_BOGUS_ARRAY_HEADER_FORMAT */
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 2 /* 0x2 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        &__midl_frag30
    }
};

static const __midl_frag24_t __midl_frag24 =
{ 
/* kablam_command_status_result */
    { 
    /* kablam_command_status_result */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* kablam_command_status_result */
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 2952 /* 0xb88 */,
        0,
        0,
        0,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag36
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag36
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag36
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag36
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x4,    /* FC64_INT16 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag29
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag73
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag73
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 2 /* 0x2 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag73
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag35
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag36
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag37
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag23_t __midl_frag23 =
{ 
/* *kablam_command_status_result */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag24
};

static const __midl_frag22_t __midl_frag22 =
{ 
/* kablam_command_status_rpc */
    { 
    /* kablam_command_status_rpc */      /* procedure kablam_command_status_rpc */
        (NDR64_UINT32) 131138 /* 0x20042 */,    /* primitive handle */ /* IsIntrepreted, ServerMustSize */
        (NDR64_UINT32) 8 /* 0x8 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 1 /* 0x1 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* result */      /* parameter result */
        &__midl_frag24,
        { 
        /* result */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    }
};

static const __midl_frag16_t __midl_frag16 =
{ 
/* kablam_command_skip_result */
    { 
    /* kablam_command_skip_result */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* kablam_command_skip_result */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 260 /* 0x104 */
    }
};

static const __midl_frag15_t __midl_frag15 =
{ 
/* *kablam_command_skip_result */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag16
};

static const __midl_frag14_t __midl_frag14 =
{ 
/* kablam_command_skip_rpc */
    { 
    /* kablam_command_skip_rpc */      /* procedure kablam_command_skip_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 8 /* 0x8 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 364 /* 0x16c */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 1 /* 0x1 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* result */      /* parameter result */
        &__midl_frag16,
        { 
        /* result */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    }
};

static const __midl_frag13_t __midl_frag13 =
{ 
/* kablam_command_kill_server_rpc */
    { 
    /* kablam_command_kill_server_rpc */      /* procedure kablam_command_kill_server_rpc */
        (NDR64_UINT32) 66 /* 0x42 */,    /* primitive handle */ /* IsIntrepreted */
        (NDR64_UINT32) 0 /* 0x0 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */
    }
};

static const __midl_frag10_t __midl_frag10 =
{ 
/* *char */
    { 
    /* *char */
        0x63,    /* FC64_CONF_CHAR_STRING */
        { 
        /* *char */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT16) 1 /* 0x1 */
    },
    &__midl_frag9
};

static const __midl_frag9_t __midl_frag9 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x5,    /* FC64_INT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */  /* Offset */
    }
};

static const __midl_frag8_t __midl_frag8 =
{ 
/* *char */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag10
};

static const __midl_frag6_t __midl_frag6 =
{ 
/* *char */
    { 
    /* *char */
        0x63,    /* FC64_CONF_CHAR_STRING */
        { 
        /* *char */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT16) 1 /* 0x1 */
    },
    &__midl_frag5
};

static const __midl_frag5_t __midl_frag5 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x5,    /* FC64_INT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 0 /* 0x0 */  /* Offset */
    }
};

static const __midl_frag4_t __midl_frag4 =
{ 
/* *char */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag6
};

static const __midl_frag2_t __midl_frag2 =
{ 
/* kablam_command_get_version_rpc */
    { 
    /* kablam_command_get_version_rpc */      /* procedure kablam_command_get_version_rpc */
        (NDR64_UINT32) 4325442 /* 0x420042 */,    /* primitive handle */ /* IsIntrepreted, ServerMustSize, ClientCorrelation */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* unk_version_length */      /* parameter unk_version_length */
        &__midl_frag217,
        { 
        /* unk_version_length */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        0 /* 0x0 */,   /* Stack offset */
    },
    { 
    /* unk_version */      /* parameter unk_version */
        &__midl_frag6,
        { 
        /* unk_version */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* build_string_length */      /* parameter build_string_length */
        &__midl_frag217,
        { 
        /* build_string_length */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* build_string */      /* parameter build_string */
        &__midl_frag10,
        { 
        /* build_string */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* version_int */      /* parameter version_int */
        &__midl_frag217,
        { 
        /* version_int */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1_t __midl_frag1 =
(NDR64_UINT32) 0 /* 0x0 */;
#ifdef __cplusplus
}
#endif


#include "poppack.h"


static const FormatInfoRef kablam_commands_interface_Ndr64ProcTable[] =
    {
    &__midl_frag2,
    &__midl_frag13,
    &__midl_frag14,
    &__midl_frag128,
    &__midl_frag22,
    &__midl_frag50,
    &__midl_frag128,
    &__midl_frag50,
    &__midl_frag128,
    &__midl_frag62,
    &__midl_frag68,
    &__midl_frag75,
    &__midl_frag128,
    &__midl_frag82,
    &__midl_frag91,
    &__midl_frag98,
    &__midl_frag101,
    &__midl_frag107,
    &__midl_frag113,
    &__midl_frag119,
    &__midl_frag124,
    &__midl_frag128,
    &__midl_frag133,
    &__midl_frag137,
    &__midl_frag147,
    &__midl_frag157,
    &__midl_frag172,
    &__midl_frag172,
    &__midl_frag177,
    &__midl_frag180,
    &__midl_frag190,
    &__midl_frag197,
    &__midl_frag211,
    &__midl_frag206,
    &__midl_frag211,
    &__midl_frag215
    };


#ifdef __cplusplus
namespace {
#endif
static const MIDL_STUB_DESC kablam_commands_interface_StubDesc = 
    {
    (void *)& kablam_commands_interface___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &kablam_commands_IfHandle,
    0,
    0,
    0,
    0,
    kablam_commands__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x60001, /* Ndr library version */
    0,
    0x8010274, /* MIDL Version 8.1.628 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x2000001, /* MIDL flag */
    0, /* cs routines */
    (void *)& kablam_commands_interface_ProxyInfo,   /* proxy/server info */
    0
    };
#ifdef __cplusplus
}
#endif

static const MIDL_SYNTAX_INFO kablam_commands_interface_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    kablam_commands__MIDL_ProcFormatString.Format,
    kablam_commands_interface_FormatStringOffsetTable,
    kablam_commands__MIDL_TypeFormatString.Format,
    0,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) kablam_commands_interface_Ndr64ProcTable,
    0,
    0,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO kablam_commands_interface_ProxyInfo =
    {
    &kablam_commands_interface_StubDesc,
    kablam_commands__MIDL_ProcFormatString.Format,
    kablam_commands_interface_FormatStringOffsetTable,
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax_2_0,
    2,
    (MIDL_SYNTAX_INFO*)kablam_commands_interface_SyntaxInfo
    
    };

#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

