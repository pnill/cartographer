#include "stdafx.h"

#include "screen_cartographer_menus.h"

#include "H2MOD/GUI/ImGui_Integration/imgui_handler.h"

#include "H2MOD/Modules/CustomMenu/CustomMenuGlobals.h"

#include "H2MOD/Modules/Updater/Updater.h"

void* ui_load_cartographer_guide_menu()
{
	return ui_custom_cartographer_load_menu(c_cartographer_guide_menu::open);
}

void* ui_load_cartographer_update_notice_menu()
{
	return ui_custom_cartographer_load_menu(c_cartographer_update_notice_menu::open);
}

void* ui_load_cartographer_credits_menu()
{
	return ui_custom_cartographer_load_menu(c_cartographer_credits_menu::open);
}

void* ui_load_cartographer_update_menu()
{
	return ui_custom_cartographer_load_menu(c_cartographer_update_menu::open);
}

void* c_cartographer_guide_menu::open(s_screen_parameters* parameters)
{
	c_cartographer_guide_menu* guide_menu = nullptr;
	uint8* ui_buffer = (uint8*)ui_pool_allocate_space(sizeof(c_cartographer_guide_menu), 0);

	if (ui_buffer) {
		guide_menu = new (ui_buffer) c_cartographer_guide_menu(
			parameters->m_channel_type, parameters->m_window_index, parameters->user_flags);
		guide_menu->m_allocated = true;
	}

	user_interface_register_screen_to_channel(guide_menu, parameters);
	return guide_menu;
}

c_cartographer_guide_edit_list::c_cartographer_guide_edit_list(uint16 _flags) :
	c_list_widget(_flags),
	field_2C0(0),
	m_slot_2(this, &c_cartographer_guide_edit_list::button_handler)
{
	s_data_array* account_list_data = ui_list_data_new("cartographer guide list", 4, 4);
	this->m_list_data = account_list_data;

	data_make_valid(account_list_data);

	for (int32 i = 0; i < this->m_list_data->datum_max_elements; i++) {
		datum_new(this->m_list_data);
	}

	// no fucking clue what's this, maybe related to this->m_slot_2.c_slot_vtbl data offset
	// because this->m_slot_2.c_slot_vtbl is at offset 708
	if ((void*)this == (void*)-(int32)offsetof(c_cartographer_guide_edit_list, m_slot_2)) {
		linker_type2.link(nullptr);
	}
	else {
		linker_type2.link(&this->m_slot_2);
	}
}

c_cartographer_guide_menu::c_cartographer_guide_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags) :
	c_screen_with_menu(_screen_brightness_level, _ui_channel, _window_index, _flags, &m_guide_edit_list),
	m_guide_edit_list(_flags)
{
}

void c_cartographer_guide_edit_list::button_handler(s_event_record* event_record, int32* a3)
{
	int32 button_id = DATUM_INDEX_TO_ABSOLUTE_INDEX(*a3);

	if (button_id == 0) 
	{
		ImGuiHandler::ToggleWindow(k_advanced_settings_window_name);
	}
	else if (button_id == 1) 
	{
		ShellExecuteA(NULL, "open", "https://cartographer.online/", NULL, NULL, SW_SHOWDEFAULT);
	}
	else if (button_id == 2) 
	{
		ui_load_cartographer_credits_menu();
	}
	else if (button_id == 3) 
	{
		ui_load_cartographer_update_menu();
	}
}

void* c_cartographer_credits_menu::open(s_screen_parameters* parameters)
{
	c_cartographer_credits_menu* credits_menu = nullptr;
	uint8* ui_buffer = (uint8*)ui_pool_allocate_space(sizeof(c_cartographer_credits_menu), 0);

	if (ui_buffer) {
		credits_menu = new (ui_buffer) c_cartographer_credits_menu(
			parameters->m_channel_type, parameters->m_window_index, parameters->user_flags);
		credits_menu->m_allocated = true;
	}

	user_interface_register_screen_to_channel(credits_menu, parameters);
	return credits_menu;
}

c_cartographer_credits_edit_list::c_cartographer_credits_edit_list(uint16 _flags) :
	c_list_widget(_flags),
	field_2C0(0),
	m_slot_2(this, &c_cartographer_credits_edit_list::button_handler)
{
	s_data_array* account_list_data = ui_list_data_new("cartographer credits list", 16, 4);
	this->m_list_data = account_list_data;

	data_make_valid(account_list_data);

	for (int32 i = 0; i < this->m_list_data->datum_max_elements; i++) {
		datum_new(this->m_list_data);
	}

	// no fucking clue what's this, maybe related to this->m_slot_2.c_slot_vtbl data offset
	// because this->m_slot_2.c_slot_vtbl is at offset 708
	if ((void*)this == (void*)-(int32)offsetof(c_cartographer_guide_edit_list, m_slot_2)) {
		linker_type2.link(nullptr);
	}
	else {
		linker_type2.link(&this->m_slot_2);
	}
}

c_cartographer_credits_menu::c_cartographer_credits_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags) :
	c_screen_with_menu(_screen_brightness_level, _ui_channel, _window_index, _flags, &m_credits_edit_list),
	m_credits_edit_list(_flags)
{
}

void c_cartographer_credits_edit_list::button_handler(s_event_record* event_record, int32* a3)
{
	return;
}

void* c_cartographer_update_menu::open(s_screen_parameters* parameters)
{
	c_cartographer_update_menu* update_menu = nullptr;
	uint8* ui_buffer = (uint8*)ui_pool_allocate_space(sizeof(c_cartographer_update_menu), 0);

	if (ui_buffer) {
		update_menu = new (ui_buffer) c_cartographer_update_menu(
			parameters->m_channel_type, parameters->m_window_index, parameters->user_flags);
		update_menu->m_allocated = true;
	}

	user_interface_register_screen_to_channel(update_menu, parameters);
	return update_menu;
}

c_cartographer_update_edit_list::c_cartographer_update_edit_list(uint16 _flags) :
	c_list_widget(_flags),
	field_2C0(0),
	m_slot_2(this, &c_cartographer_update_edit_list::button_handler)
{
	s_data_array* account_list_data = ui_list_data_new("cartographer update list", 2, 4);
	this->m_list_data = account_list_data;

	data_make_valid(account_list_data);

	for (int32 i = 0; i < this->m_list_data->datum_max_elements; i++) {
		datum_new(this->m_list_data);
	}

	// no fucking clue what's this, maybe related to this->m_slot_2.c_slot_vtbl data offset
	// because this->m_slot_2.c_slot_vtbl is at offset 708
	if ((void*)this == (void*)-(int32)offsetof(c_cartographer_guide_edit_list, m_slot_2)) {
		linker_type2.link(nullptr);
	}
	else {
		linker_type2.link(&this->m_slot_2);
	}

	// ### TODO FIXME this is pretty hacked up, cleanup the system at some point
	m_keep_screen_open = true;
	m_update_status = _cartographer_update_status_pending_check_for_update;

	// check the updater state, and set the current state accordingly
	if (updater_has_files_to_download)
	{
		m_update_status = _cartographer_update_status_pending_download_update;
	}

	if (updater_has_files_to_install)
	{
		m_update_status = _cartographer_update_status_pending_install_update;
	}
}

c_cartographer_update_menu::c_cartographer_update_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags) :
	c_screen_with_menu(_screen_brightness_level, _ui_channel, _window_index, _flags, &m_update_edit_list),
	m_update_edit_list(_flags)
{
	GSDownloadInit();
}

void c_cartographer_update_edit_list::button_handler(s_event_record* event_record, int32* a3)
{
	int32 button_id = DATUM_INDEX_TO_ABSOLUTE_INDEX(*a3);

	e_user_interface_render_window	parent_render_window = this->get_parent_render_window();
	e_user_interface_channel_type	parent_screen_ui_channel = this->get_parent_channel();

	if (button_id == 0)
	{
		switch (m_update_status)
		{
		case _cartographer_update_status_pending_check_for_update:
			GSDownloadCheck();
			m_update_status = _cartographer_update_status_checking_for_update;
			break;
		case _cartographer_update_status_pending_download_update:
			if (updater_has_files_to_download) {
				GSDownloadDL();
				m_update_status = _cartographer_update_status_downloading_update;
			}
			break;
		case _cartographer_update_status_pending_install_update:
			if (updater_has_files_to_install) {
				GSDownloadInstall();
				m_update_status = _cartographer_update_status_installing_update;
			}
			break;

		case _cartographer_update_status_checking_for_update:
		case _cartographer_update_status_downloading_update:
		default:
			break;
		}
	}
	else if (button_id == 1)
	{
		GSDownloadCancel();
		m_keep_screen_open = false;

		user_interface_back_out_from_channel(parent_screen_ui_channel, parent_render_window);
	}

	return;
}

void* c_cartographer_update_notice_menu::open(s_screen_parameters* parameters)
{
	c_cartographer_update_notice_menu* update_notice_menu = nullptr;
	uint8* ui_buffer = (uint8*)ui_pool_allocate_space(sizeof(c_cartographer_update_notice_menu), 0);

	if (ui_buffer) {
		update_notice_menu = new (ui_buffer) c_cartographer_update_notice_menu(
			parameters->m_channel_type, parameters->m_window_index, parameters->user_flags);
		update_notice_menu->m_allocated = true;
	}

	user_interface_register_screen_to_channel(update_notice_menu, parameters);
	return update_notice_menu;
}

c_cartographer_update_notice_edit_list::c_cartographer_update_notice_edit_list(uint16 _flags) :
	c_list_widget(_flags),
	field_2C0(0),
	m_slot_2(this, &c_cartographer_update_notice_edit_list::button_handler)
{
	s_data_array* account_list_data = ui_list_data_new("cartographer update notice list", 2, 4);
	this->m_list_data = account_list_data;

	data_make_valid(account_list_data);

	for (int32 i = 0; i < this->m_list_data->datum_max_elements; i++) {
		datum_new(this->m_list_data);
	}

	// no fucking clue what's this, maybe related to this->m_slot_2.c_slot_vtbl data offset
	// because this->m_slot_2.c_slot_vtbl is at offset 708
	if ((void*)this == (void*)-(int32)offsetof(c_cartographer_guide_edit_list, m_slot_2)) {
		linker_type2.link(nullptr);
	}
	else {
		linker_type2.link(&this->m_slot_2);
	}
}

c_cartographer_update_notice_menu::c_cartographer_update_notice_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags) :
	c_screen_with_menu(_screen_brightness_level, _ui_channel, _window_index, _flags, &m_update_notice_edit_list),
	m_update_notice_edit_list(_flags)
{
}

void c_cartographer_update_notice_edit_list::button_handler(s_event_record* event_record, int32* a3)
{
	int32 button_id = DATUM_INDEX_TO_ABSOLUTE_INDEX(*a3);

	e_user_interface_render_window	parent_render_window = this->get_parent_render_window();
	e_user_interface_channel_type	parent_screen_ui_channel = this->get_parent_channel();

	if (button_id == 0) 
	{
		ui_load_cartographer_update_menu();
	}
	else if (button_id == 1)
	{
		user_interface_back_out_from_channel(parent_screen_ui_channel, parent_render_window);
	}

	return;
}

