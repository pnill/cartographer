

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for kablam_rpc\kablam_commands.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, app_config, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __kablam_commands_h_h__
#define __kablam_commands_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __kablam_commands_interface_INTERFACE_DEFINED__
#define __kablam_commands_interface_INTERFACE_DEFINED__

/* interface kablam_commands_interface */
/* [implicit_handle][version][uuid] */ 

#pragma region Enums
typedef /* [public][public][public] */ 
enum __MIDL_kablam_commands_interface_0001
    {
        _skip_result_code_match_skipped	= 0,
        _skip_result_code_match_ended	= 1,
        _skip_result_code_match_ending_waiting	= 2,
        _skip_result_code_server_not_active	= 3,
        k_skip_result_code_count	= ( _skip_result_code_server_not_active + 1 ) 
    } 	e_kablam_command_skip_code;

typedef /* [public][public][public] */ 
enum __MIDL_kablam_commands_interface_0002
    {
        _status_result_code_offine_initializing	= 0,
        _status_result_code_offline_product_key_required	= 1,
        _status_result_code_offline_invalid_product_key	= 2,
        _status_result_code_server_banned	= 3,
        _status_result_code_update_required	= 4,
        _status_result_code_offline	= 5,
        _status_result_code_signing_in	= 6,
        _status_result_code_signin_out	= 7,
        _status_result_code_connecting	= 8,
        _status_result_code_offline_waiting_to_reconnect	= 9,
        _status_result_code_offline_signin_failed	= 10,
        _status_result_code_offline_disconnected	= 11,
        _status_result_code_online_no_playlist	= 12,
        _status_result_code_online_failed_to_load_playlist	= 13,
        _status_result_code_online_empty_playlist	= 14,
        _status_result_code_online_pregame_lobby	= 15,
        _status_result_code_online_starting_game	= 16,
        _status_result_code_online_game_in_progress	= 17,
        _status_result_code_online_game_over	= 18,
        _status_result_code_online_postgame_carnage_report	= 19,
        k_status_result_code_count	= ( _status_result_code_online_postgame_carnage_report + 1 ) 
    } 	e_kablam_command_status_result_code;

typedef /* [public][public][public][v1_enum] */ 
enum __MIDL_kablam_commands_interface_0003
    {
        _status_response_flag_lobby_info	= 0,
        _status_response_flag_live	= 1,
        _status_response_flag_xlive_signin_result	= 2,
        _status_response_flag_signin_mode	= 3,
        _status_response_flag_has_gamertag	= 4,
        _status_response_flag_has_lan_name	= 5,
        _status_response_flag_has_playlist_path	= 6,
        k_status_response_flag_count	= ( _status_response_flag_has_playlist_path + 1 ) 
    } 	e_kablam_command_status_response_flags;

typedef /* [public][public] */ 
enum __MIDL_kablam_commands_interface_0004
    {
        _kick_player_response_player_kicked	= 0,
        _kick_player_response_server_not_running	= 1,
        _kick_player_response_player_not_found	= 2,
        k_kick_player_response_count	= ( _kick_player_response_player_not_found + 1 ) 
    } 	kablam_command_kick_result_code;

typedef /* [public][public][public][public][public] */ 
enum __MIDL_kablam_commands_interface_0005
    {
        _name_result_code_success	= 0,
        _name_result_code_lan_only	= 1,
        _name_result_code_invalid_utf16	= 2,
        k_name_result_code_count	= ( _name_result_code_invalid_utf16 + 1 ) 
    } 	e_kablam_command_set_unicode_setting_result_code;

typedef /* [public][public][public] */ 
enum __MIDL_kablam_commands_interface_0006
    {
        _play_error_playlist_read_failed	= 0,
        _play_error_file_not_found	= 1,
        _play_error_file_open_access_denied	= 2,
        _play_error_playlist_no_valid_matches	= 4,
        k_play_error_count	= ( _play_error_playlist_no_valid_matches + 1 ) 
    } 	e_kablam_command_play_error_code;

typedef /* [public][public][public][public] */ 
enum __MIDL_kablam_commands_interface_0007
    {
        _play_warning_duplicate_playlist_section	= 0,
        _play_warning_duplicate_variant_name	= 1,
        _play_warning_error_unknown_setting	= 2,
        _play_warning_error_unknown_section	= 3,
        _play_warning_error_invalid_setting_value	= 4,
        _play_warning_error_setting_value_already_set	= 5,
        _play_warning_error_match_missing_setting	= 6,
        _play_warning_error_invalid_variant_setting	= 7,
        _play_warning_error_variant_missing_type_or_base	= 8,
        _play_warning_error_variant_missing_name	= 9,
        _play_warning_error_variant_invalid_utf16	= 10,
        _play_warning_error_fariant_pipe_forbidden	= 11,
        _play_warning_error_variant_invalid_base_variant	= 12,
        _play_warning_error_variant_both_type_and_base	= 13,
        _play_warning_error_variant_missing_or_invalid	= 14,
        _play_warning_error_map_missing_or_invalid	= 15,
        _play_warning_warn_variants_overflow	= 16,
        _play_warning_warn_matches_overflow	= 17,
        k_play_warning_count	= ( _play_warning_warn_matches_overflow + 1 ) 
    } 	e_kablam_command_play_warning_type;

typedef /* [public][public] */ 
enum __MIDL_kablam_commands_interface_0008
    {
        _send_message_result_code_sent	= 0,
        _send_message_result_code_server_not_active	= 2,
        _send_message_result_code_no_players_in_session	= 3,
        _send_message_result_code_message_failed	= 4,
        k_send_message_result_code_count	= ( _send_message_result_code_message_failed + 1 ) 
    } 	e_kablam_command_send_message_result_code;

typedef /* [public][public] */ 
enum __MIDL_kablam_commands_interface_0009
    {
        _live_key_response_code_product_key_set	= 0,
        _live_key_response_code_key_invalid	= 1,
        _live_key_response_code_key_set_failed	= 2,
        k_live_key_response_code_count	= ( _live_key_response_code_key_set_failed + 1 ) 
    } 	e_kablam_command_live_key_response_code;

typedef /* [public][public] */ 
enum __MIDL_kablam_commands_interface_0010
    {
        _live_auto_signin_response_code_auto_signin_enabled	= 0,
        _live_auto_signin_response_code_auto_signin_failed	= 1,
        _live_auto_signin_response_code_live_only	= 2,
        k_live_auto_signin_response_code_count	= ( _live_auto_signin_response_code_live_only + 1 ) 
    } 	e_kablam_command_live_auto_signin_response_code;

typedef /* [public][public] */ 
enum __MIDL_kablam_commands_interface_0011
    {
        _live_signin_result_code_success	= 0,
        _live_signin_result_code_live_only	= 1,
        k_live_signin_result_code_count	= ( _live_signin_result_code_live_only + 1 ) 
    } 	e_kablam_command_live_signin_result_code;

typedef /* [public][public] */ 
enum __MIDL_kablam_commands_interface_0012
    {
        _live_signout_result_code_signing_out_live	= 0,
        _live_signout_result_code_not_signed_in	= 1,
        _live_signout_result_code_live_only	= 2,
        k_live_signout_result_code_count	= ( _live_signout_result_code_live_only + 1 ) 
    } 	e_kablam_command_live_signout_result_code;

typedef /* [public][public][public][public][public] */ 
enum __MIDL_kablam_commands_interface_0013
    {
        _ban_network_result_code_success	= 0,
        _ban_network_result_code_lan_only	= 1,
        _ban_network_result_code_ban_list_full	= 2,
        _ban_network_result_code_ban_not_found	= 3,
        k_ban_network_result_code_count	= ( _ban_network_result_code_ban_not_found + 1 ) 
    } 	e_kablam_command_ban_network_result_code;

typedef /* [public][public][public][public][public] */ 
enum __MIDL_kablam_commands_interface_0014
    {
        _gamer_table_result_code_success	= 0,
        _gamer_table_result_code_live_only	= 1,
        _gamer_table_result_code_table_full	= 2,
        _gamer_table_result_code_gamer_not_found	= 3,
        k_gamer_table_result_code_count	= ( _gamer_table_result_code_gamer_not_found + 1 ) 
    } 	e_kablam_command_gamer_table_result_code;

typedef /* [public][public] */ 
enum __MIDL_kablam_commands_interface_0015
    {
        _unban_all_ip	= 0,
        _unban_all_nic	= 1,
        _unban_all_gamertags	= 2,
        k_unban_all_count	= ( _unban_all_gamertags + 1 ) 
    } 	e_kablam_unban_all_type;

typedef /* [public][public] */ 
enum __MIDL_kablam_commands_interface_0016
    {
        _unban_all_result_code_success	= 0,
        _unban_all_result_code_failed	= 1,
        k_unban_all_result_code_count	= ( _unban_all_result_code_failed + 1 ) 
    } 	e_kablam_command_unban_all_result_code;

typedef /* [public][public] */ 
enum __MIDL_kablam_commands_interface_0017
    {
        _vip_clear_result_code_success	= 0,
        _vip_clear_result_code_live_only	= 1,
        k_vip_clear_result_code_count	= ( _vip_clear_result_code_live_only + 1 ) 
    } 	e_kablam_command_vip_clear_result_code;

typedef /* [public][public][public] */ 
enum __MIDL_kablam_commands_interface_0018
    {
        _stats_folder_result_code_success	= 0,
        _stats_folder_result_code_invalid	= 1,
        _stats_folder_result_code_not_writable	= 2,
        _stats_folder_result_code_export_disabled	= 3,
        k_stats_folder_result_code_count	= ( _stats_folder_result_code_export_disabled + 1 ) 
    } 	e_kablam_command_stats_folder_result_code;

typedef /* [public][public][public] */ 
enum __MIDL_kablam_commands_interface_0019
    {
        _kablam_command_privacy_open	= 0,
        _kablam_command_privacy_gold	= 1,
        _kablam_command_privacy_vip	= 2,
        k_kablam_command_privacy_type_count	= ( _kablam_command_privacy_vip + 1 ) ,
        _kablam_command_privacy_none	= -1
    } 	e_kablam_command_privacy_type;

typedef /* [public][public][public] */ 
enum __MIDL_kablam_commands_interface_0020
    {
        _privacy_result_code_success	= 0,
        _privacy_result_code_live_only	= 1,
        k_privacy_result_code_count	= ( _privacy_result_code_live_only + 1 ) 
    } 	e_kablam_command_privacy_result_code;

typedef /* [public][public] */ 
enum __MIDL_kablam_commands_interface_0021
    {
        _set_max_players_result_code_success	= 0,
        _set_max_players_result_code_out_of_range	= 1,
        k_set_max_players_result_code_count	= ( _set_max_players_result_code_out_of_range + 1 ) 
    } 	e_kablam_command_set_max_players_result_code;

#pragma endregion
#pragma region Structures
typedef struct kablam_command_skip_result
    {
    e_kablam_command_skip_code result_code;
    wchar_t playing_variant[ 32 ];
    wchar_t playing_map[ 32 ];
    wchar_t next_variant[ 32 ];
    wchar_t next_map[ 32 ];
    } 	kablam_command_skip_result;

typedef struct kablam_command_status_result
    {
    e_kablam_command_status_result_code result_code;
    e_kablam_command_status_response_flags flags;
    long game_time_remaining;
    long game_time_in_seconds;
    wchar_t playing_variant[ 32 ];
    wchar_t playing_map[ 32 ];
    wchar_t next_variant[ 32 ];
    wchar_t next_map[ 32 ];
    short player_count;
    wchar_t player_names[ 16 ][ 32 ];
    wchar_t playlist_path[ 256 ];
    wchar_t custom_map_path[ 256 ];
    long xlive_signin_result_code;
    wchar_t live_id[ 256 ];
    unsigned char gamer_tag[ 16 ];
    wchar_t server_description[ 32 ];
    wchar_t lan_server_name[ 16 ];
    } 	kablam_command_status_result;

typedef struct kablam_command_playlist_warning
    {
    e_kablam_command_play_warning_type warning_type;
    unsigned long line_number;
    wchar_t parameter[ 32 ];
    wchar_t value[ 32 ];
    wchar_t help[ 32 ];
    } 	kablam_command_playlist_warning;

typedef struct kablam_command_play_result
    {
    short playlist_load_result;
    e_kablam_command_play_error_code error_code;
    wchar_t loaded_playlist_path[ 256 ];
    struct kablam_command_playlist_warning warnings[ 200 ];
    unsigned long warning_count;
    long possibly_unused;
    } 	kablam_command_play_result;

typedef struct s_ipv4_subnet
    {
    long ipv4_address;
    unsigned char cidr;
    } 	s_ipv4_subnet;

typedef struct s_mac_address
    {
    unsigned char octets[ 6 ];
    } 	s_mac_address_rpc;

typedef struct s_gamertag
    {
    wchar_t gamertag[ 16 ];
    } 	s_gamertag;

typedef struct s_exipration_entry_ipv4_subnet
    {
    hyper expiry_time_stamp;
    struct s_ipv4_subnet ipv4_address;
    } 	s_exipration_entry_ipv4_subnet;

typedef struct s_expiration_entry_mac_address
    {
    hyper expiry_time_stamp;
    struct s_mac_address mac_address;
    } 	s_expiration_entry_mac_address;

typedef struct s_expiration_entry_gamer
    {
    hyper expiry_time_stamp;
    struct s_gamertag gamer_tag;
    } 	s_expiration_entry_gamer;

#pragma endregion
#pragma region Functions
void kablam_command_get_version_rpc( 
    /* [in] */ long unk_version_length,
    /* [size_is][string][out] */ unsigned char *unk_version,
    /* [in] */ long build_string_length,
    /* [size_is][string][out] */ unsigned char *build_string,
    /* [out] */ long *version_int);

void kablam_command_kill_server_rpc( void);

void kablam_command_skip_rpc( 
    /* [out] */ struct kablam_command_skip_result *result);

void kablam_command_kick_rpc( 
    /* [string][in] */ wchar_t *gamer_tag,
    /* [ref][out] */ kablam_command_kick_result_code *result);

void kablam_command_status_rpc( 
    /* [out] */ struct kablam_command_status_result *result);

void kablam_command_get_name_rpc( 
    /* [ref][out] */ e_kablam_command_set_unicode_setting_result_code *result_code,
    /* [in] */ long buffer_length,
    /* [size_is][string][out] */ wchar_t *out_name);

void kablam_command_set_name_rpc( 
    /* [string][in] */ wchar_t *name,
    /* [ref][out] */ e_kablam_command_set_unicode_setting_result_code *result_code);

void kablam_command_get_description_rpc( 
    /* [ref][out] */ e_kablam_command_set_unicode_setting_result_code *result_code,
    /* [in] */ long buffer_length,
    /* [size_is][string][out] */ wchar_t *out_description);

void kablam_command_set_description_rpc( 
    /* [string][in] */ wchar_t *description,
    /* [ref][out] */ e_kablam_command_set_unicode_setting_result_code *result_code);

void kablam_command_send_message_rpc( 
    /* [size_is][string][in] */ wchar_t *message,
    /* [ref][out] */ e_kablam_command_send_message_result_code *result_code);

void kablam_command_play_rpc( 
    /* [string][in] */ wchar_t *file_path,
    /* [out] */ struct kablam_command_play_result *out_response);

void kablam_command_playing_rpc( 
    /* [out] */ struct kablam_command_play_result *out_response);

void kablam_command_live_activate_rpc( 
    /* [string][in] */ wchar_t *key,
    /* [ref][out] */ e_kablam_command_live_key_response_code *result_code);

void kablam_command_live_auto_signin_rpc( 
    /* [string][in] */ wchar_t *username,
    /* [string][in] */ wchar_t *password,
    /* [ref][out] */ e_kablam_command_live_auto_signin_response_code *result_code,
    /* [out] */ unsigned long *xlive_login_result);

void kablam_command_live_signin_rpc( 
    /* [string][in] */ wchar_t *username,
    /* [string][in] */ wchar_t *password,
    /* [ref][out] */ e_kablam_command_live_signin_result_code *result_code);

void kablam_command_live_signout_rpc( 
    /* [ref][out] */ e_kablam_command_live_signout_result_code *result_code);

void kablam_command_ban_ip_rpc( 
    /* [in] */ long ipv4_address,
    /* [in] */ unsigned char subnet,
    /* [in] */ long duration,
    /* [ref][out] */ e_kablam_command_ban_network_result_code *result_code);

void kablam_command_ban_nic_rpc( 
    /* [in] */ struct s_mac_address *mac_address,
    /* [in] */ long duration,
    /* [ref][out] */ e_kablam_command_ban_network_result_code *result_code);

void kablam_command_ban_gamer_rpc( 
    /* [string][in] */ wchar_t *gamer_tag,
    /* [in] */ long duration,
    /* [ref][out] */ e_kablam_command_gamer_table_result_code *result_code);

void kablam_command_unban_ip_rpc( 
    /* [in] */ long ipv4_address,
    /* [in] */ unsigned char cidr,
    /* [ref][out] */ e_kablam_command_ban_network_result_code *result_code);

void kablam_command_unban_nic_rpc( 
    /* [in] */ struct s_mac_address *mac_address,
    /* [ref][out] */ e_kablam_command_ban_network_result_code *result_code);

void kablam_command_unban_gamer_rpc( 
    /* [string][in] */ wchar_t *gamer_tag,
    /* [ref][out] */ e_kablam_command_gamer_table_result_code *result_code);

void kablam_command_unban_all_rpc( 
    /* [in] */ e_kablam_unban_all_type unban_all_type,
    /* [ref][out] */ e_kablam_command_unban_all_result_code *result_code);

void kablam_command_get_ban_ip_rpc( 
    /* [out] */ unsigned long *returned_results_count,
    /* [out] */ long *returned_results_size,
    /* [size_is][size_is][ref][out] */ struct s_exipration_entry_ipv4_subnet **buffer);

void kablam_command_get_ban_nic_rpc( 
    /* [out] */ unsigned long *returned_results_count,
    /* [out] */ long *returned_results_size,
    /* [size_is][size_is][ref][out] */ struct s_expiration_entry_mac_address **buffer);

void kablam_command_get_ban_gamer_rpc( 
    /* [out] */ unsigned long *returned_results_count,
    /* [out] */ long *returned_results_size,
    /* [size_is][size_is][ref][out] */ struct s_expiration_entry_gamer **buffer);

void kablam_command_vip_add_rpc( 
    /* [string][in] */ wchar_t *gamer_tag,
    /* [ref][out] */ e_kablam_command_gamer_table_result_code *result_code);

void kablam_command_vip_remove_rpc( 
    /* [string][in] */ wchar_t *gamer_tag,
    /* [ref][out] */ e_kablam_command_gamer_table_result_code *result_code);

void kablam_command_vip_clear_rpc( 
    /* [ref][out] */ e_kablam_command_vip_clear_result_code *arg_1);

void kablam_command_get_vip_rpc( 
    /* [out] */ unsigned long *returned_results_count,
    /* [out] */ long *returned_results_size,
    /* [size_is][size_is][ref][out] */ struct s_gamertag **buffer);

void kablam_command_get_stats_folder_rpc( 
    /* [ref][out] */ e_kablam_command_stats_folder_result_code *result_code,
    /* [in] */ long buffer_length,
    /* [size_is][string][out] */ wchar_t *out_path);

void kablam_command_set_stats_folder_rpc( 
    /* [string][in] */ wchar_t *in_path,
    /* [ref][out] */ e_kablam_command_stats_folder_result_code *result_code);

void kablam_command_set_privacy_rpc( 
    /* [in] */ e_kablam_command_privacy_type privacy_type,
    /* [ref][out] */ e_kablam_command_privacy_result_code *result_code);

void kablam_command_get_privacy_rpc( 
    /* [ref][out] */ e_kablam_command_privacy_type *out_privacy,
    /* [ref][out] */ e_kablam_command_privacy_result_code *result_code);

void kablam_command_set_max_players_rpc( 
    /* [in] */ unsigned int player_count,
    /* [ref][out] */ e_kablam_command_set_max_players_result_code *result_code);

void kablam_command_get_max_players_rpc( 
    /* [out] */ unsigned long *out_player_count);

#pragma endregion

extern handle_t kablam_commands_IfHandle;


extern RPC_IF_HANDLE kablam_commands_interface_v1_0_c_ifspec;
extern RPC_IF_HANDLE kablam_commands_interface_v1_0_s_ifspec;
#endif /* __kablam_commands_interface_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


