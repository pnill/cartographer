#pragma once
#include "loader.hpp"
#include "Blam/Cache/TagGroups/object_definition.hpp"

class object_loader : public BlamLoader
{
public:
	virtual size_t get_total_size()
	{
		int size = 0;
		auto object = (s_object_group_definition*)this->base_data;

		size += object->ai_properties.data_size();
		size += object->functions.data_size();
		size += object->attachments.data_size();
		size += object->widgets.data_size();
		size += object->old_functions.data_size();
		size += object->change_colors.data_size();
		for (auto i = 0; i < object->change_colors.size; i++)
		{
			s_object_group_definition::s_change_colors_block temp;
			map_stream->seekg(resolve_data_offset(object->change_colors.data) + i * sizeof(s_object_group_definition::s_change_colors_block));
			map_stream->read((char*)&temp, sizeof(s_object_group_definition::s_change_colors_block));
			size += temp.initial_permutations.data_size();
			size += temp.functions.data_size();
		}
		size += object->predicted_resources.data_size();
		return size;
	}
	virtual void rebase(int base)
	{
		LOG_INFO_GAME("[{}] {}", __FUNCTION__, base);
		auto object = (s_object_group_definition*)this->data;
		object->ai_properties.data = base + (object->ai_properties.data - instance->data_offset);
		object->functions.data = base + (object->functions.data - instance->data_offset);
		object->attachments.data = base + (object->attachments.data - instance->data_offset);
		object->widgets.data = base + (object->widgets.data - instance->data_offset);
		object->old_functions.data = base + (object->old_functions.data - instance->data_offset);
		object->change_colors.data = base + (object->change_colors.data - instance->data_offset);
		auto colors = reinterpret_cast<s_object_group_definition::s_change_colors_block*>(&this->data[object->change_colors.data]);
		for(auto i = 0; i < object->change_colors.size; i++)
		{
			colors[i].functions.data = base + (colors[i].functions.data - instance->data_offset);
			colors[i].initial_permutations.data = base + (colors[i].initial_permutations.data - instance->data_offset);
		}
		object->predicted_resources.data = base + (object->predicted_resources.data - instance->data_offset);
	}

	//virtual char* get_tag_data(int base)
	//{
		/*auto bobject = (s_object_group_definition*)this->base_data;
		auto object = (s_object_group_definition*)this->data;
		for(auto i = 0; i < bobject->ai_properties.size; i++)
		{
			map_stream->seekg(resolve_data_offset(bobject->ai_properties.data) + i * sizeof(s_object_group_definition::s_ai_properties_block));
			map_stream->read(&this->data[used_bytes + i * sizeof(s_object_group_definition::s_ai_properties_block)], sizeof(s_object_group_definition::s_ai_properties_block));
		}
		object->ai_properties.data = used_bytes;
		used_bytes += bobject->ai_properties.data_size();

		for(auto i = 0; i < bobject->functions.size; i++)
		{
			map_stream->seekg(resolve_data_offset(bobject->functions.data) + i * sizeof(s_object_group_definition::s_functions_block));
			map_stream->read(&this->data[used_bytes + i * sizeof(s_object_group_definition::s_functions_block)], sizeof(s_object_group_definition::s_functions_block));
		}
		object->functions.data = used_bytes;
		used_bytes += bobject->functions.data_size();

		for(auto i = 0; i < bobject->attachments.size; i++)
		{
			map_stream->seekg(resolve_data_offset(bobject->attachments.data) + i * sizeof(s_object_group_definition::s_attachments_block));
			map_stream->read(&this->data[used_bytes + i * sizeof(s_object_group_definition::s_attachments_block)], sizeof(s_object_group_definition::s_attachments_block));
		}
		object->attachments.data = used_bytes;
		used_bytes += bobject->attachments.data_size();

		for(auto i = 0; i < bobject->widgets.size; i++)
		{
			map_stream->seekg(resolve_data_offset(bobject->widgets.data) + i * sizeof(s_object_group_definition::s_widgets_block));
			map_stream->read(&this->data[used_bytes + i * sizeof(s_object_group_definition::s_widgets_block)], sizeof(s_object_group_definition::s_widgets_block));
		}
		object->attachments.data = used_bytes;
		used_bytes += bobject->widgets.data_size();

		for(auto i = 0; i < bobject->old_functions.size; i++)
		{
			map_stream->seekg(resolve_data_offset(bobject->old_functions.data) + i * sizeof(s_object_group_definition::s_old_functions_block));
			map_stream->read(&this->data[used_bytes + i * sizeof(s_object_group_definition::s_old_functions_block)], sizeof(s_object_group_definition::s_old_functions_block));
		}
		object->old_functions.data = used_bytes;
		used_bytes += bobject->old_functions.data_size();

		for(auto i = 0; i < bobject->change_colors.size; i++)
		{
			map_stream->seekg(resolve_data_offset(bobject->change_colors.data) + i * sizeof(s_object_group_definition::s_change_colors_block));

		}*/
	//}
};
