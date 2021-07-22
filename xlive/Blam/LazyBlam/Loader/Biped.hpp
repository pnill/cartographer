#pragma once
#include "Unit.hpp"
#include "Blam/Cache/TagGroups/biped_definition.hpp"

class biped_loader : public unit_loader
{
public:
	virtual size_t get_total_size() override
	{
		int size = 0;
		auto biped = (s_biped_group_definition*)this->base_data;

		size += biped->dead_sphere_shapes.data_size();
		size += biped->pill_shapes.data_size();
		size += biped->sphere_shapes.data_size();
		size += biped->contact_points.data_size();
		size += sizeof(s_biped_group_definition);
		size += this->unit_loader::get_total_size();
		return size;
	}
	virtual void rebase(int base) override
	{
		auto a = 0;
		auto biped = (s_biped_group_definition*)*this->data;
		unit_loader::rebase(base);;
		L_BLAM_LOADER_REBASE(biped->dead_sphere_shapes);
		L_BLAM_LOADER_REBASE(biped->pill_shapes);
		L_BLAM_LOADER_REBASE(biped->sphere_shapes);
		L_BLAM_LOADER_REBASE(biped->contact_points);
	}
	s_biped_group_definition* get_tag_data(int base)
	{
		auto size = get_total_size();
		*this->data = new char[size];
		this->current_position = sizeof(s_biped_group_definition);
		map_stream->seekg(resolve_data_offset(instance->data_offset));
		map_stream->read(*this->data, size);
		rebase(base);
		return (s_biped_group_definition*)*this->data;
	}
};
