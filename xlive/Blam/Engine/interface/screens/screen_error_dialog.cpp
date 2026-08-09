#include "stdafx.h"
#include "screen_error_dialog.h"

#include "input/input_windows.h"
#include "interface/user_interface.h"
#include "interface/user_interface_controller.h"

#include "H2MOD/Modules/Accounts/AccountLogin.h"

static decltype(c_screen_error_dialog_ok::show_dialog)* p_error_ok_dialog_show;
static decltype(c_screen_error_dialog_ok_cancel::show_dialog)* p_ok_cancel_dialog_show;

void* __cdecl c_screen_error_dialog_ok::load_for_active_users(
	c_screen_parameters* parameters)
{
	if ((parameters->get_user_flags() & 0xFF) == (uint8)NONE)
	{
		parameters->set_user_flag_unsafe(user_interface_controller_get_signed_in_controllers_mask() | FLAG(k_windows_device_controller_index));
	}

	return INVOKE(0x20E032, 0x0, c_screen_error_dialog_ok::load_for_active_users, parameters);
}

void* c_screen_error_dialog_ok::show_dialog(
	e_user_interface_channel_type channel_type,
	e_ui_error_types error_type,
	e_user_interface_render_window window_index,
	uint16 user_flags,
	void* ok_callback,
	void* fallback)
{
	switch (error_type)
	{
	case _ui_error_install_not_complete:
		LOG_TRACE_FUNC("ignoring need to reinstall maps");
		return NULL;
	case _ui_error_booted_from_session:
	{
		// boot them offline.
		XUserSignOut(0);
		UpdateMasterLoginStatus();
	}
	}

	return p_error_ok_dialog_show(channel_type, error_type, window_index, user_flags, ok_callback, fallback);
}

void c_screen_error_dialog_ok::apply_patches()
{
	WritePointer(Memory::GetAddress(0x20E173) + 1, c_screen_error_dialog_ok::load_for_active_users);
	DETOUR_ATTACH(p_error_ok_dialog_show, Memory::GetAddress<decltype(p_error_ok_dialog_show)>(0x20E15A), c_screen_error_dialog_ok::show_dialog);
}

void* c_screen_error_dialog_ok_cancel::show_dialog(
	e_user_interface_channel_type channel_type,
	e_ui_error_types error_type,
	e_user_interface_render_window window_index,
	uint16 user_flags,
	void* ok_callback, 
	void* fallback,
	int32 a7,
	int32 a8)
{
	//ALL_USERS_MASK
	if ((user_flags & 0xFF) == (uint8)NONE)
	{
		switch (error_type)
		{
		case _ui_error_overwrite_custom_keyboard_mappings:
		case _ui_error_map_download_collision:
		case _ui_error_live_checkpoint_connection_dropped:
		case _ui_error_live_checkpoint_hash_mismatch:
		case _ui_error_tnp_no_source:
		case _ui_error_tnp_disk_read:
		case _ui_error_tnp_no_engine_running:
		case _ui_error_tnp_signature_verification:
		case _ui_error_tnp_drive_removed:
		case _ui_error_tnp_disk_full:
		case _ui_error_tnp_permissions:
		case _ui_error_tnp_unknown:
			//special cases and cant be supported by all controllers
			user_flags = user_interface_controller_get_signed_in_controllers_mask();
			break;
		case _ui_error_confirm_boot_to_dash:
		case _ui_error_confirm_save_and_exit_campaign:
			user_flags = user_interface_controller_get_signed_in_controllers_mask()
				| FLAG(k_windows_device_controller_index); // addtitionally needed for start_screen_introduction
			break;
		case _ui_error_add_to_favorites:
		case _ui_error_remove_from_favorites:
			// these work fine with ALL_USERS_MASK
			break;
		}
	}

	return p_ok_cancel_dialog_show(channel_type, error_type, window_index, user_flags, ok_callback, fallback, a7, a8);
}

void c_screen_error_dialog_ok_cancel::apply_patches()
{
	DETOUR_ATTACH(p_ok_cancel_dialog_show, Memory::GetAddress<decltype(p_ok_cancel_dialog_show)>(0x20E243), c_screen_error_dialog_ok_cancel::show_dialog);
}
