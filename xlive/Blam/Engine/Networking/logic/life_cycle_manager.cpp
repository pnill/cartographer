#include "stdafx.h"
#include "life_cycle_manager.h"

#include "Util/Hooks/Hook.h"

e_game_life_cycle get_game_life_cycle()
{
	typedef e_game_life_cycle(__cdecl* get_lobby_state_t)();
	auto p_get_game_life_cycle = Memory::GetAddress<get_lobby_state_t>(0x1AD660, 0x1A65DD);

	return p_get_game_life_cycle();
}

void c_game_life_cycle_handler::initialize(void* life_cycle_manager, e_game_life_cycle life_cycle, bool unk_bool)
{
	this->life_cycle_manager = life_cycle_manager;
	this->life_cycle = life_cycle;
	this->unk_bool = unk_bool;
	static_cast<c_game_life_cycle_manager*>(this->life_cycle_manager)->life_cycle_handlers[this->life_cycle] = this;
}

bool c_game_life_cycle_manager::game_life_cycle_initialized()
{
	return *Memory::GetAddress<bool*>(0x420FC0, 0x3C40A8);
}

c_game_life_cycle_manager* c_game_life_cycle_manager::get()
{
	return Memory::GetAddress<c_game_life_cycle_manager*>(0x420FC4);
}

e_game_life_cycle c_game_life_cycle_manager::get_life_cycle() const
{
	if (game_life_cycle_initialized())
		return life_cycle_state;
	return _life_cycle_none;
}

bool c_game_life_cycle_manager::state_is_joining() const
{
	if (!game_life_cycle_initialized())
		return false;
	if (life_cycle_state == _life_cycle_joining)
		return true;
	return false;
}

bool c_game_life_cycle_manager::state_is_in_game() const
{
	if (!game_life_cycle_initialized())
		return false;
	if (life_cycle_state == _life_cycle_in_game)
		return true;
	return false;
}

void c_game_life_cycle_manager::request_state_change(e_game_life_cycle requested_state, int unk_int, void* unk_ptr)
{
	this->requested_life_cycle = requested_state;
	this->update_requested = true;
	this->field_3C = unk_int;
	this->field_40 = 0;
	if (this->field_3C > 0)
		memcpy(&field_40, unk_ptr, field_3C);
}

void c_game_life_cycle_manager::update()
{
	//live_list_get_servers();
	bool do_work = true;
	do
	{
		if(this->update_requested)
		{
			this->update_requested = false;
			void* unk_ptr = 0;
			if (this->field_3C >= 0)
				unk_ptr = this->field_40;

			this->life_cycle_changing = true;
			if(this->requested_life_cycle != this->life_cycle_state)
			{
				c_game_life_cycle_handler* current_handler = this->life_cycle_handlers[this->life_cycle_state];
				c_game_life_cycle_handler* requested_handler = this->life_cycle_handlers[this->requested_life_cycle];
				current_handler->functions->dispose(current_handler, requested_handler);
				this->life_cycle_state = this->requested_life_cycle;
				requested_handler->functions->initialize(requested_handler, current_handler, this->field_3C, unk_ptr);
			}
			this->life_cycle_changing = false;
		}
		else
		{
			this->life_cycle_updating = true;
			auto handler = this->life_cycle_handlers[this->life_cycle_state];
			//crashes here trying to access handler->life_cycle_manager->network_session
			do_work = handler->functions->update(handler);
			this->life_cycle_updating = false;
		}
	}
	while (do_work);
}



namespace game_life_cycle
{
	void life_cycle_update()
	{
		if (c_game_life_cycle_manager::game_life_cycle_initialized())
		{
			auto cycle = c_game_life_cycle_manager::get();
			cycle->update();
		}
	}
	void apply_patches()
	{
		PatchCall(Memory::GetAddress(0x39C9C), life_cycle_update);
	}
}
