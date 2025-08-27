#include "stdafx.h"
#include "screen_custom_game_profile_select.h"

#include "screen_custom_game_map_select.h"
#include "screen_error_dialog.h"
#include "screen_game_engine_category.h"
#include "screen_multiplayer_pregame_lobby.h"
#include "screen_saved_game_file_actions.h"
#include "screen_virtual_keyboard.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_globals.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_networking.h"
#include "interface/user_interface_shared_globals.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/session/network_session.h"
#include "saved_games/saved_game_files.h"
#include "tag_files/global_string_ids.h"
#include "text/unicode.h"

#define create_game_engine_load_functions(name, save_type) \
void* c_screen_custom_game_profile_select::load_##name##_settings(s_screen_parameters* parameters) { return load_settings(parameters, (save_type));  } \
void* c_screen_custom_game_profile_select::load_##name##_unused(s_screen_parameters* parameters){ return load_unused(parameters, (save_type)); } \
void* c_screen_custom_game_profile_select::load_##name##_lobby(s_screen_parameters* parameters) { return load_lobby(parameters, (save_type)); }

c_custom_game_profile_list::c_custom_game_profile_list(int16 user_flags) :
	c_list_widget(user_flags),
	enumerated_file_index_storage{},
	m_slot(this, &c_custom_game_profile_list::handle_item_pressed_event)
{
	m_list_data = ui_list_data_new(k_custom_game_profile_list_name, k_maximum_enumerated_total_save_game_files + 1,
		sizeof(s_custom_game_profile_list_item));

	data_make_valid(m_list_data);

	this->save_game_type = _saved_game_file_type_game_variant_slayer;
	this->unk_1 = NONE;
	this->enumerated_index = NONE;
	this->data_2[0] = 0;
	this->data_2[1] = 0;
	this->unk_bool = false;
	this->data_3 = 0;

	linker_type2.link(&this->m_slot);
}

void c_custom_game_profile_list::handle_item_pressed_event(s_event_record** pevent, datum* pitem_index)
{
	if (*pitem_index == NONE)
		return;

	s_saved_game_files_globals* files_globals = saved_game_files_globals_get();

	ASSERT(files_globals);

	s_custom_game_profile_list_item* selected_item = (s_custom_game_profile_list_item*)datum_get(this->m_list_data, *pitem_index);
	s_game_variant variant{};


	if (selected_item->enumerated_index != NONE &&
		(!this->data_2[0] || !ENUMERATED_INDEX_IS_DEFAULT_SAVE(selected_item->enumerated_index)))
	{
		if (selected_item->unk_1 != NONE)
		{
			if (saved_game_load_game_variant(selected_item->enumerated_index, &variant))
			{
				if (this->data_2[0])
				{
					user_interface_set_variant(selected_item->enumerated_index, &variant);
					s_screen_parameters params;
					params.m_context = nullptr;
					params.data_new(
						0,
						FLAG((*pevent)->controller),
						_user_interface_channel_type_gameshell_dialog,
						_window_4,
						c_screen_saved_game_file_actions::load_settings);

					params.m_load_function(&params);

					return;
				}
				if (this->data_2[1])
				{
					s_screen_parameters params;
					params.m_context = nullptr;
					params.data_new(
						0,
						FLAG((*pevent)->controller),
						_user_interface_channel_type_gameshell_screen,
						_window_4,
						c_screen_custom_game_map_select::load_unused);

					params.m_load_function(&params);

					return;
				}

				c_network_session* network_session{};
				network_life_cycle_in_squad_session(&network_session);

				if (!network_session->established() || ENUMERATED_INDEX_IS_DEFAULT_SAVE(selected_item->enumerated_index))
				{
					user_interface_game_settings_set_game_variant(&variant);
				}
				else
				{
					c_screen_multiplayer_pregame_lobby::globals_set_variant(&variant);
					c_screen_multiplayer_pregame_lobby::globals_set_unk_flag(false);
				}

				if (this->unk_bool)
				{
					s_saved_game_player_profile* user_interface_profile = user_interface_globals_get_edit_player_profile();

					if (ENUMERATED_INDEX_IS_DEFAULT_SAVE(selected_item->enumerated_index))
						user_interface_profile->last_selected_file_index = selected_item->enumerated_index;
					else
						user_interface_profile->last_selected_file_index = NONE;

					ustrncpy(user_interface_profile->last_selected_variant_name, variant.variant_name, NUMBEROF(variant.variant_name));

					user_interface_globals_save_edit_profile_to_disk();

					user_interface_profile->last_selected_variant_set = true;
				}
				user_interface_back_out_from_channel(this->get_parent_channel(), this->get_parent_render_window());
				return;
			}
			this->data_3 = 1;
		}
		s_event_record* t_record = *pevent;
		c_screen_custom_game_profile_select::set_global_enumerated_index(selected_item->enumerated_index);
		user_interface_error_ok_cancel_dialog_show_confirmation(
			_user_interface_channel_type_game_error,
			_window_4,
			1 << ((*pevent)->controller),
			nullptr, // todo: rewrite the mess of usercalls FO: 0x251578
			_ui_error_confirm_corrupt_game_variant
		);
		return;
	}

	files_globals->saved_file_creation_result = _saved_gave_disk_result_success;

	wchar_t saved_game_file_name[128]{};

	if (saved_game_create_save_game_directory(this->save_game_type, saved_game_file_name))
	{
		enumerated_file_index new_variant_index = saved_game_create_new_game_variant(
			(*pevent)->controller,
			this->save_game_type,
			saved_game_file_name
		);

		if (new_variant_index == NONE)
		{
			user_interface_set_variant(new_variant_index, &variant);
			s_game_variant* ui_variant = user_interface_get_variant();

			user_interface_construct_default_game_variant_from_file_type(ui_variant, this->save_game_type);
			ui_load_virtual_keyboard_variant(
				(*pevent)->controller,
				_vkbd_context_variant_name_entry,
				saved_game_get_variant_file_type(ui_variant),
				ui_variant->variant_name,
				32);
			return;
		}
		else if (saved_game_load_game_variant(new_variant_index, &variant))
		{
			user_interface_set_variant(new_variant_index, &variant);
			s_game_variant* ui_variant = user_interface_get_variant();

			ui_variant->flags &= ~1u;

			user_interface_construct_default_game_variant_from_file_type(ui_variant, this->save_game_type);
			ui_load_virtual_keyboard_variant(
				(*pevent)->controller,
				_vkbd_context_variant_name_entry,
				saved_game_get_variant_file_type(ui_variant),
				ui_variant->variant_name,
				32);
			return;
		}
	}

	c_screen_error_dialog_ok::load_for_disk_result(this->m_controllers_mask, files_globals->saved_file_creation_result);

	//INVOKE_TYPE(0x251B7E, 0, void(__thiscall*)(c_custom_game_profile_list*, s_event_record**, datum*), this, pevent, pitem_index);
}

void c_custom_game_profile_list::perform_save_type_search()
{
	enumerated_file_index search_results[4197]{};
	uint32 search_result_count = 0;

	saved_game_files_search_by_type(
		_controller_index_0,
		this->save_game_type,
		&search_result_count,
		search_results,
		true);

	data_make_invalid(this->m_list_data);

	memset(this->enumerated_file_index_storage, 0xFF, sizeof(this->enumerated_file_index_storage));

	if (this->data_2[0])
	{
		s_custom_game_profile_list_item* creation_item = (s_custom_game_profile_list_item*)datum_new(this->m_list_data);
		creation_item->enumerated_index = NONE;
		creation_item->description_text = _string_id_create_new_description_text;
		creation_item->unk_1 = 0;
	}

	search_result_count = PIN(search_result_count, 0, k_maximum_enumerated_total_save_game_files);

	if (search_result_count != 0)
	{
		s_game_variant search_variant{};
		for (uint32 index = 0; index < search_result_count; ++index)
		{
			if (saved_game_load_game_variant(search_results[index], &search_variant) &&
				(ENUMERATED_INDEX_IS_DEFAULT_SAVE(search_results[index]) ||
					!uniswcntrl(search_variant.variant_name) &&
					!wchar_has_double_pipe(search_variant.variant_name)))
			{
				s_custom_game_profile_list_item* next_item = (s_custom_game_profile_list_item*)datum_new(this->m_list_data);
				next_item->enumerated_index = search_results[index];
				next_item->unk_1 = 0;
				next_item->description_text = _string_id_none;

				if (ENUMERATED_INDEX_IS_DEFAULT_SAVE(search_results[index]))
				{
					uint8 description_index = PIN((uint8)(search_variant.description_index), 0, k_maximum_enumerated_default_save_game_files - 1);
					if (description_index == (uint8)search_variant.description_index)
						next_item->description_text = string_id_number(description_index);
				}
				this->enumerated_file_index_storage[index] = search_results[index];
			}
		}
	}
	this->setup_item_indices();
}

void c_custom_game_profile_list::sub_6515A6()
{
	INVOKE_TYPE(0x2515A6, 0, void(__thiscall*)(c_custom_game_profile_list*), this);
}

void c_custom_game_profile_list::setup_children()
{
	//this->perform_save_type_search();
	//this->sub_613E7C((s_list_item_datum*)datum_next(this->m_list_data, NONE));
	//user_interface_clear_variant();

	//int32 result = c_list_widget::setup_children();

	//if (!this->data_2[0] && !this->data_2[1])
	//	sub_6515A6();

	//this->field_A2 = 0;

	//return result;
	INVOKE_TYPE(0x251AE9, 0, void(__thiscall*)(c_custom_game_profile_list*), this);
}

void c_custom_game_profile_list::update()
{
	INVOKE_TYPE(0x251B35, 0, void(__thiscall*)(c_custom_game_profile_list*), this);
}

c_list_item_widget* c_custom_game_profile_list::get_list_items()
{
	return this->m_list_items;
}

int32 c_custom_game_profile_list::get_list_items_count()
{
	return k_game_profile_list_item_count;
}

void c_custom_game_profile_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	INVOKE_TYPE(0x2513B6, 0, void(__thiscall*)(c_custom_game_profile_list*, c_list_item_widget*, int32), this, item, skin_index);
}

void* c_screen_custom_game_profile_select::load_settings(s_screen_parameters* parameters, e_saved_game_file_type type)
{
	c_screen_custom_game_profile_select* screen;

	void* pool = ui_pool_allocate_space(21648, 0);

	if (pool)
	{
		screen = new (pool) c_screen_custom_game_profile_select(
			parameters->m_channel_type,
			parameters->m_window_index,
			parameters->m_user_flags,
			_screen_custom_game_menu
		);

		screen->m_allocated = true;

		screen->save_game_type = type;

		screen->unk_data[0] = 1;
		screen->unk_data[1] = 0;

		screen->m_profile_list.unk_bool = parameters->m_context != nullptr;

		user_interface_register_screen_to_channel((c_screen_with_menu*)screen, parameters);
	}
	else
	{
		screen = nullptr;
	}

	return nullptr;
}

void* c_screen_custom_game_profile_select::load_lobby(s_screen_parameters* parameters, e_saved_game_file_type type)
{
	c_screen_custom_game_profile_select* screen;

	void* pool = ui_pool_allocate_space(21648, 0);

	if (pool)
	{
		screen = new (pool) c_screen_custom_game_profile_select(
			parameters->m_channel_type,
			parameters->m_window_index,
			parameters->m_user_flags,
			_screen_multiplayer_variant_listlobby
		);

		screen->m_allocated = true;

		screen->save_game_type = type;

		screen->unk_data[0] = 0;
		screen->unk_data[1] = 0;

		screen->m_profile_list.unk_bool = parameters->m_context != nullptr;

		user_interface_register_screen_to_channel((c_screen_with_menu*)screen, parameters);
	}
	else
	{
		screen = nullptr;
	}

	return nullptr;
}

void* c_screen_custom_game_profile_select::load_unused(s_screen_parameters* parameters, e_saved_game_file_type type)
{
	c_screen_custom_game_profile_select* screen;

	void* pool = ui_pool_allocate_space(21648, 0);

	if (pool)
	{
		screen = new (pool) c_screen_custom_game_profile_select(
			parameters->m_channel_type,
			parameters->m_window_index,
			parameters->m_user_flags,
			_screen_custom_game_menu
		);

		screen->m_allocated = true;

		screen->save_game_type = type;

		screen->unk_data[0] = 0;
		screen->unk_data[1] = 1;

		screen->m_profile_list.unk_bool = parameters->m_context != nullptr;

		user_interface_register_screen_to_channel((c_screen_with_menu*)screen, parameters);
	}
	else
	{
		screen = nullptr;
	}

	return nullptr;
}

void c_screen_custom_game_profile_select::set_global_enumerated_index(enumerated_file_index enumerated_file_index)
{
	WriteValue(Memory::GetAddress(0x978E28), enumerated_file_index);
}

create_game_engine_load_functions(slayer, _saved_game_file_type_game_variant_slayer);
create_game_engine_load_functions(king, _saved_game_file_type_game_variant_koth);
create_game_engine_load_functions(oddball, _saved_game_file_type_game_variant_oddball);
create_game_engine_load_functions(juggernaut, _saved_game_file_type_game_variant_juggernaut);
create_game_engine_load_functions(ctf, _saved_game_file_type_game_variant_ctf);
create_game_engine_load_functions(assault, _saved_game_file_type_game_variant_assault);
create_game_engine_load_functions(territories, _saved_game_file_type_game_variant_territories);
create_game_engine_load_functions(headhunter, _saved_game_file_type_game_variant_headhunter);

void c_screen_custom_game_profile_select::update()
{
	INVOKE_TYPE(0x25188C, 0, void(__thiscall*)(c_screen_custom_game_profile_select*), this);
}

void c_screen_custom_game_profile_select::sub_60E884()
{
	this->m_profile_list.save_game_type = this->save_game_type;
	this->m_profile_list.data_2[0] = this->unk_data[0];
	this->m_profile_list.data_2[1] = this->unk_data[1];
}

const void* c_screen_custom_game_profile_select::load_proc() const
{
	switch (this->save_game_type)
	{
	case _saved_game_file_type_game_variant_slayer:
	{
		if (this->unk_data[0])
			return &this->load_slayer_settings;
		if (this->unk_data[1])
			return &this->load_slayer_unused;

		return &this->load_slayer_lobby;
	}
	case _saved_game_file_type_game_variant_koth:
	{
		if (this->unk_data[0])
			return &this->load_king_settings;
		if (this->unk_data[1])
			return &this->load_king_unused;

		return &this->load_king_lobby;
	}
	case _saved_game_file_type_game_variant_oddball:
	{
		if (this->unk_data[0])
			return &this->load_oddball_settings;
		if (this->unk_data[1])
			return &this->load_oddball_unused;

		return &this->load_oddball_lobby;
	}
	case _saved_game_file_type_game_variant_juggernaut:
	{
		if (this->unk_data[0])
			return &this->load_juggernaut_settings;
		if (this->unk_data[1])
			return &this->load_juggernaut_unused;

		return &this->load_juggernaut_lobby;
	}
	case _saved_game_file_type_game_variant_ctf:
	{
		if (this->unk_data[0])
			return &this->load_ctf_settings;
		if (this->unk_data[1])
			return &this->load_ctf_unused;

		return &this->load_ctf_lobby;
	}
	case _saved_game_file_type_game_variant_assault:
	{
		if (this->unk_data[0])
			return &this->load_assault_settings;
		if (this->unk_data[1])
			return &this->load_assault_unused;

		return &this->load_assault_lobby;
	}
	case _saved_game_file_type_game_variant_territories:
	{
		if (this->unk_data[0])
			return &this->load_territories_settings;
		if (this->unk_data[1])
			return &this->load_territories_unused;

		return &this->load_territories_lobby;
	}
	case _saved_game_file_type_game_variant_headhunter:
	{
		if (this->unk_data[0])
			return &this->load_headhunter_settings;
		if (this->unk_data[1])
			return &this->load_headhunter_unused;

		return &this->load_headhunter_lobby;
	}
	default:
		return &this->load_slayer_settings;
	}
}

c_screen_custom_game_profile_select::c_screen_custom_game_profile_select(e_user_interface_channel_type ui_channel, e_user_interface_render_window window_index, uint16 user_flags, e_user_interface_screen_id screen_id) :
	c_screen_with_menu(screen_id, ui_channel, window_index, user_flags, &m_profile_list),
	m_profile_list(user_flags),
	unk_data{}
{
	this->save_game_type = _saved_game_file_type_game_variant_slayer;
}

#undef create_game_engine_load_functions