#pragma once
#include "interface/user_interface_widget_list.h"
#include "saved_games/player_profile.h"


struct s_user_interface_player_profile_storage
{
	int32 enumerated_file_index;
	s_saved_game_player_profile profile;
	int8 data_120C[100];
};


class c_player_profile_list : public c_list_widget
{
public:
	static void apply_instance_patches();


	// c_player_profile_list virtual functions

	virtual ~c_player_profile_list();
	virtual int32 setup_children() override;
	virtual void on_screen_leave() override;
	virtual void update() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

	// c_player_profile_list additions

	virtual s_user_interface_player_profile_storage* get_displayed_player_profile_storage(uint32* out_size) = 0;
	virtual void unknown_function_2(int32 a1);
	virtual void update_displayed_profiles();
	virtual wchar_t* unknown_function_3(int32 a1);

private:
	template<typename T>
	static T get_base_vtable_fn_ptr(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddressRelative<void**>(0x3CB7C4)[idx]);
	}
};

class c_screen_single_player_profile_select_fancy
{
public:
	static void* load(s_screen_parameters* parameters);
};

