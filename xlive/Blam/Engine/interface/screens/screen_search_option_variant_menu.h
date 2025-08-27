#pragma once
#include "interface/user_interface_widget.h"
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

class c_screen_search_option_variant_menu : public c_screen_with_menu
{
public:
	// overrides(function_index, function_address, computed_name, overridden_function)

	//0: 0x653eaa - c_screen_search_option_variant_menu::deconstructor - c_screen_with_menu::deconstructor
	virtual ~c_screen_search_option_variant_menu() = default;

	//23: 0x653e24 - c_screen_search_option_variant_menu::post_initialize - c_screen_widget::post_initialize
	virtual void post_initialize() override;

	//38: 0x65464a - c_screen_search_option_variant_menu::load_proc - c_screen_with_menu::load_proc
	const void* load_proc() const override;

};