#include "MetaExtender.h"
#include "Blam/Cache/TagGroups/new_hud_definition.hpp"
#include "H2MOD/Tags/TagInterface.h"

std::vector<void*> ToFree;


namespace MetaExtender {
	template<typename T = void>
	void move_child_block(unsigned long tag_block_ptr)
	{
		int* block_count = reinterpret_cast<int*>(tag_block_ptr);
		int* block_offset = reinterpret_cast<int*>(tag_block_ptr + 4);
		size_t block_size = *block_count * sizeof(T);
		void* new_memory = calloc(*block_count + 1, sizeof(T));
		ToFree.push_back(new_memory);
		memcpy(new_memory, &tags::get_tag_data()[*block_offset], block_size);
		if (visit_struct::traits::is_visitable<T>::value)
		{
			for (auto i = 0; i < *block_count; i++)
			{
				const T* t_block = reinterpret_cast<T*>(((unsigned long)new_memory) + (sizeof(T) * i));
				visit_struct::for_each(*t_block, [](const char *name, const auto &value)
				{
					if (std::string(typeid(value).name()).find("tag_block") != std::string::npos)
					{
						auto a = reinterpret_cast<decltype(value)>(value);
						LOG_INFO_GAME(IntToString<unsigned long>((unsigned long)std::addressof(value), std::hex));

						if (a.size != 0)
						{
							move_child_block<decltype(a[0])>((unsigned long)std::addressof(value));
						}
					}
				});
			}
		}
	}
	//Currently only works with 1 level deep reflexives.
	void add_tag_block(unsigned long tag_block_ptr, int tag_block_item_size, int tag_block_item_count, int newCount)
	{
		size_t currentSize = tag_block_item_size * tag_block_item_count;

		void* new_tag_block = calloc(tag_block_item_count + newCount, tag_block_item_size);

		ToFree.push_back(new_tag_block);

		memcpy(new_tag_block, &tags::get_tag_data()[*reinterpret_cast<int*>(tag_block_ptr + 4)], currentSize);

		*reinterpret_cast<int*>(tag_block_ptr) = tag_block_item_count + newCount;
		*reinterpret_cast<int*>(tag_block_ptr + 4) = (unsigned long)new_tag_block - int(*Memory::GetAddress<int**>(0x47CD54));
	}


	//template<typename T = void>
	//T* add_tag_block2(unsigned long tag_block_ptr)
	//{
	//	int* block_count = reinterpret_cast<int*>(tag_block_ptr);
	//	int* block_offset = reinterpret_cast<int*>(tag_block_ptr + 4);
	//	size_t block_size = *block_count * sizeof(T);
	//	void* new_memory = calloc(*block_count + 1, sizeof(T));
	//	ToFree.push_back(new_memory);

	//	memcpy(new_memory, &tags::get_tag_data()[*block_offset], block_size);

	//	*block_count = *block_count + 1;
	//	*block_offset = (int)((unsigned long)new_memory - int(*Memory::GetAddress<int**>(0x47CD54)));

	//	return reinterpret_cast<T*>(((unsigned long)new_memory) + (sizeof(T) * (*block_count - 1)));
	//}
	template<typename T = void>
	T* add_tag_block3(tag_block<T>* block)
	{
		size_t block_size = block->size * sizeof(T);
		void* new_memory = calloc(block->size + 1, sizeof(T));
		ToFree.push_back(new_memory);

		memcpy(new_memory, &tags::get_tag_data()[block->data], block_size);

		block->size = block->size + 1;
		block->data = (int)((unsigned long)new_memory - int(*Memory::GetAddress<int**>(0x47CD54)));

		return reinterpret_cast<T*>(((unsigned long)new_memory) + (sizeof(T) * (block->size - 1)));
	}



	void test()
	{
		auto chief_hud_datum = tags::find_tag(blam_tag::tag_group_type::newhuddefinition, "ui\\hud\\masterchief");
		auto chief_hud_tag = tags::get_tag<blam_tag::tag_group_type::newhuddefinition, s_new_hud_definition_group_definition>(chief_hud_datum);

		auto new_text_widget = MetaExtender::add_tag_block2<s_new_hud_definition_group_definition::s_text_widgets_block>((unsigned long)std::addressof(chief_hud_tag->text_widgets));
		new_text_widget->name = chief_hud_tag->text_widgets[0]->name;
		new_text_widget->input_1 = s_new_hud_definition_group_definition::e_input::unit_frag_gren_cnt;
		new_text_widget->enable_unit_flags = s_new_hud_definition_group_definition::s_text_widgets_block::enable_unit_flags::unit_is_unzoomed;
		new_text_widget->disable_unit_flags = s_new_hud_definition_group_definition::s_text_widgets_block::disable_unit_flags::binoculars_enabled;
		new_text_widget->age_cutoff = 0;
		new_text_widget->clip_cutoff = 4;
		new_text_widget->total_cutoff = 4;
		new_text_widget->anchor = s_new_hud_definition_group_definition::s_text_widgets_block::e_anchor::weapon_hud;
		new_text_widget->shader = chief_hud_tag->text_widgets[0]->shader;
		new_text_widget->string = chief_hud_tag->text_widgets[0]->string;
		new_text_widget->justification = s_new_hud_definition_group_definition::s_text_widgets_block::e_justification::center;
		new_text_widget->fullscreen_offset_x = 130;
		new_text_widget->fullscreen_offset_y = -10;
		new_text_widget->halfscreen_offset_x = 65;
		new_text_widget->halfscreen_offset_y = -5;
		new_text_widget->quarterscreen_offset_x = 65;
		new_text_widget->quarterscreen_offset_y = -5;
		LOG_INFO_GAME(IntToString<unsigned long>((unsigned long)std::addressof(*new_text_widget), std::hex));


		//auto wigt = tags::find_tag(blam_tag::tag_group_type::userinterfacescreenwidgetdefinition, "ui\\screens\\game_shell\\settings_screen\\player_profile\\button_settings");
		//auto t_wigt = tags::get_tag<blam_tag::tag_group_type::userinterfacescreenwidgetdefinition, s_user_interface_screen_widget_definition>(wigt);

		//auto new_panel = MetaExtender::add_tag_block2<s_user_interface_screen_widget_definition::s_panes_block>((unsigned long)std::addressof(t_wigt->panes));
		//auto new_list_block = MetaExtender::add_tag_block2<s_user_interface_screen_widget_definition::s_panes_block::s_list_block_block>((unsigned long)std::addressof(new_panel->list_block));
		//new_list_block->skin_index = s_user_interface_screen_widget_definition::s_panes_block::s_list_block_block::e_skin_index::default;
		//new_list_block->animation_index = s_user_interface_screen_widget_definition::s_panes_block::s_list_block_block::e_animation_index::NUM_22;
		//new_list_block->num_visible_items = 5;
		//new_list_block->bottom_left_x = -490;
		//new_list_block->bottom_left_y = 365;
		//new_list_block->intro_animation_delay_milliseconds = 0;

		//auto new_text_blocks
		//

		//if (t_wigt != nullptr)
		//{
		//	tags::tag_data_block* menu_content = reinterpret_cast<tags::tag_data_block*>(t_wigt + 0x20);
		//	MetaExtender::add_tag_block((unsigned long)std::addressof(menu_content), 76, 8, 1);

		//}


		/*auto bull = tags::find_tag(blam_tag::tag_group_type::biped, "objects\\characters\\masterchief\\masterchief_mp");
		auto t_bull = tags::get_tag<blam_tag::tag_group_type::biped, s_biped_group_definition>(bull);*/
		//auto scnr = tags::find_tag(blam_tag::tag_group_type::scenario, "scenarios\\multi\\cyclotron\\cyclotron");
		//auto t_scnr = tags::get_tag<blam_tag::tag_group_type::scenario, s_scenario_group_definition>(scnr);
		//auto new_decoration = MetaExtender::add_tag_block2<s_scenario_group_definition::s_decorators_block>((unsigned long)std::addressof(t_scnr->decals));
		//t_bull->initial_velocity = 1;
		//t_bull->final_velocity = 1;
		///*MetaExtender::add_tag_block((unsigned long)std::addressof(t_bull->objectTag.attachments), 0x18, t_bull->objectTag.attachments.size, 1);
		//t_bull->objectTag.attachments[1]->type.TagGroup = blam_tag::tag_group_type::light;
		//t_bull->objectTag.attachments[1]->type.TagIndex = datum(0xEDF83393);*/
		//auto new_attachment = MetaExtender::add_tag_block2<s_object_group_definition::s_change_colors_block>((unsigned long)std::addressof(t_bull->unitTag.objectTag.change_colors));
		//auto new_attachment = MetaExtender::add_tag_block2<s_object_group_definition::s_attachments_block>(&t_bull->objectTag.attachments);
		//new_attachment->type.TagGroup = blam_tag::tag_group_type::light;
		//new_attachment->type.TagIndex = datum(0xEDF83393);
	}

	void free_tag_blocks()
	{
		for (auto &block : ToFree)
		{
			free(block);
		}
		ToFree.clear();
	}

	void add_to_free(void* mem)
	{
		ToFree.push_back(mem);
	}
}
