#include "stdafx.h"
#include "simulation_entity_update_encode_helper.h"

bool c_entity_update_encode_helper::setup(c_bitstream* bitstream, uint32 required_leave_space_bits, uint32 update_component_first_index, uint32 update_component_count, uint32 update_mask)
{
	this->m_update_considered_mask = 0;
	this->m_update_written_mask = 0;
	this->m_update_overflowed_mask = 0;
	this->m_current_update_component_index = UINT_MAX;
	this->m_block_name = nullptr;

	this->m_update_mask = update_mask;
	this->m_update_component_first_index = update_component_first_index;
	this->m_required_leave_space_bits = required_leave_space_bits;
	this->m_update_component_count = update_component_count;
	this->m_bitstream = bitstream;
	this->m_current_leave_space_bits = this->m_update_component_count + this->m_required_leave_space_bits;
	this->m_able_to_write_update = bitstream->get_space_left_in_bits() >= (int32)this->m_current_leave_space_bits;

	return this->m_able_to_write_update;
}

bool c_entity_update_encode_helper::set_component_flag(uint32 component_index, const char* block_name)
{
	this->m_current_update_component_index = component_index;
	this->m_block_name = block_name;

	this->m_update_considered_mask |= FLAG(this->m_current_update_component_index);

	this->m_bitstream->push_position();

	if (TEST_BIT(this->m_update_mask, this->m_current_update_component_index))
	{
		this->m_bitstream->write_bool(this->m_block_name, true);
		this->m_update_written_mask |= FLAG(this->m_current_update_component_index);

		return true;
	}

	return false;
}

void c_entity_update_encode_helper::commit()
{
	if (TEST_BIT(this->m_update_written_mask, this->m_current_update_component_index))
	{
		if (TEST_BIT(this->m_update_skipped_mask, this->m_current_update_component_index))
		{
			this->m_update_written_mask &= ~FLAG(this->m_current_update_component_index);
			this->m_bitstream->pop_position(true);
		}
		else if (this->m_bitstream->get_space_left_in_bits() < (int32)this->m_current_leave_space_bits)
		{
			this->m_update_overflowed_mask |= FLAG(this->m_current_update_component_index);
			this->m_bitstream->pop_position(true);
		}
		else
		{
			this->m_bitstream->pop_position(false);
		}
	}
	else
	{
		this->m_bitstream->pop_position(false);
	}

	if (!TEST_BIT(this->m_update_written_mask, this->m_current_update_component_index))
		this->m_bitstream->write_bool(this->m_block_name, false);

	--this->m_current_leave_space_bits;
	this->m_current_update_component_index = UINT_MAX;
	this->m_block_name = nullptr;
}

void c_entity_update_encode_helper::set_update_mask(uint32* out_mask) const
{
	*out_mask |= this->m_update_written_mask;
}