#pragma once
#include "object.hpp"
#include "Blam/Cache/TagGroups/unit_definition.hpp"

class unit_loader : public object_loader
{
public:
	virtual size_t get_total_size() override
	{
		int size = 0;
		auto unit = (s_unit_group_definition*)this->base_data;

		size += unit->camera_tracks.data_size();
		size += unit->postures.data_size();
		size += unit->new_hud_interfaces.data_size();
		size += unit->dialogue_variants.data_size();
		size += unit->powered_seats.data_size();
		size += unit->weapons.data_size();

		size += unit->seats.data_size();
		for (auto i = 0; i < unit->seats.size; i++)
		{
			size += unit->seats[i]->camera_tracks.data_size();
			size += unit->seats[i]->unit_hud_interface.data_size();
		}
		size += this->object_loader::get_total_size();
		//size += sizeof(s_unit_group_definition) - sizeof(s_object_group_definition);
		return size;
	}
	virtual void rebase(int base) override
	{
		LOG_INFO_GAME("[{}] {}", __FUNCTION__, base);
		this->object_loader::rebase(base);
		auto unit = (s_unit_group_definition*)this->base_data;
	}
	//virtual char* get_tag_data(int base) override
	//{
	//	
	//}
};
