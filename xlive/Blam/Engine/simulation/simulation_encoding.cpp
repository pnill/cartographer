#include "stdafx.h"
#include "simulation_encoding.h"

#include "memory/bitstream.h"
#include "simulation/simulation_update.h"

/* constants */

/* enums */

/* structures */

/* prototypes */

static void __cdecl player_action_encode(c_bitstream* packet, struct player_action* action);
static bool __cdecl player_action_decode(c_bitstream* packet, struct player_action* action);
static void __cdecl simulation_machine_update_encode(c_bitstream* packet, struct simulation_machine_update* machine_update);
static bool __cdecl simulation_machine_update_decode(c_bitstream* packet, struct simulation_machine_update* machine_update);


#include "simulation/simulation_players.h"

/* public code */

void __cdecl simulation_player_update_encode(c_bitstream* packet, const simulation_player_update* player_update)
{
	ASSERT(packet);
	ASSERT(player_update);
	ASSERT(VALID_INDEX(player_update->update_type, k_simulation_player_update_type_count));

	INVOKE(0x1E06AB, 0x1C7B6B, simulation_player_update_encode, packet, player_update);
	return;
}

bool __cdecl simulation_player_update_decode(c_bitstream* packet, simulation_player_update* player_update)
{
	ASSERT(packet);
	ASSERT(player_update);

	return INVOKE(0x1E078A, 0x1C7C4A, simulation_player_update_decode, packet, player_update);
}


void __cdecl synchronous_update_encode(c_bitstream* packet, struct simulation_update* update)
{
	//INVOKE(0x1E0998, 0x0, synchronous_update_encode_internal, packet, update);

	ASSERT(packet);
	ASSERT(update);

	packet->write_integer("update-number", update->update_number, SIZEOF_BITS(update->update_number));
	packet->write_bool("simulation_in_progress", update->simulation_in_progress);//adding missing simulation_in_progress
	packet->write_integer("player-flags", update->player_action_mask, k_maximum_players);

	
	int8 player_index = 0;
	player_action* action = update->player_actions;
	do
	{
		if (TEST_BIT(update->player_action_mask, player_index))
		{
			player_action_encode(packet, action);
		}
		++player_index;
		++action;

	} while (player_index < k_maximum_players);


	// why is unit/actor control data never encoded?????
	// h3 seems to use it , but h2 does not

	packet->write_bool("machine-update-exists", update->machine_update_valid);
	if (update->machine_update_valid)
	{
		simulation_machine_update_encode(packet, &update->machine_update);
	}

	packet->write_integer("player-update-count", update->player_update_count, k_bits_required_for_simulation_player_updates_count);
	ASSERT(update->player_update_count >= 0 && update->player_update_count <= k_maximum_simulation_player_updates);


	if (update->player_update_count > 0)
	{
		int32 update_idx = 0;
		simulation_player_update* player_update = update->player_updates;
		do
		{
			simulation_player_update_encode(packet, player_update);
			++update_idx;
			++player_update;

		} while (update_idx < update->player_update_count);
	}
	packet->write_bool("flush-gamestate", update->flush_gamestate);
	packet->write_integer("verify-game-time", update->verify_game_time, SIZEOF_BITS(update->verify_game_time));
	packet->write_integer("verify-random", update->verify_random_seed, SIZEOF_BITS(update->verify_random_seed));

	// #todo
	// c_simulation_queue::encode(simulation_bookkeeping_queue);
	// c_simulation_queue::encode(game_simulation_queue);
}

bool __cdecl synchronous_update_decode(c_bitstream* packet, struct simulation_update* update)
{
	//return INVOKE(0x1E0AA2, 0x0, synchronous_update_decode_internal, packet, update);

	ASSERT(packet);
	ASSERT(update);

	bool result = true;
	update->update_number = packet->read_integer("update-number", SIZEOF_BITS(update->update_number));
	update->simulation_in_progress = packet->read_bool("simulation_in_progress"); 	//adding missing simulation_in_progress
	update->player_action_mask = packet->read_integer("player-flags", k_maximum_players);

	int8 player_index = 0;
	player_action* action = update->player_actions;
	do
	{
		if (TEST_BIT(update->player_action_mask, player_index))
		{
			result = result && player_action_decode(packet, action);
		}
		++action;
		++player_index;

	} while (player_index < k_maximum_players);


	update->machine_update_valid = packet->read_bool("machine-update-exists");
	if (update->machine_update_valid)
	{
		result = result && simulation_machine_update_decode(packet, &update->machine_update);
	}

	int32 player_update_count = packet->read_integer("player-update-count", k_bits_required_for_simulation_player_updates_count);
	update->player_update_count = player_update_count;

	if (player_update_count < 0 || player_update_count > k_maximum_simulation_player_updates)
	{
		result = false;
	}
	else
	{
		int32 update_itr = 0;
		if (player_update_count > 0)
		{
			simulation_player_update* player_update = update->player_updates;
			do
			{
				result = result && simulation_player_update_decode(packet, player_update);
				++update_itr;
				++player_update;

			} while (update_itr < player_update_count);
		}
	}
	update->flush_gamestate = packet->read_bool("flush-gamestate");
	update->verify_game_time = packet->read_integer("verify-game-time", SIZEOF_BITS(update->verify_game_time));
	update->verify_random_seed = packet->read_integer("verify-random", SIZEOF_BITS(update->verify_random_seed));


	//#todo
	//c_simulation_queue::decode(simulation_bookkeeping_queue);
	//c_simulation_queue::decode(game_simulation_queue);

	result = result && !packet->error_occurred() && update->verify_game_time > 0 && update->update_number > 0;

	if (!result)
	{
		//#todo
		//c_simulation_queue::dispose(simulation_bookkeeping_queue);
		//c_simulation_queue::dispose(game_simulation_queue);
	}

	return result;
}



/* private code */

void __cdecl player_action_encode(c_bitstream* packet, struct player_action* action)
{
	INVOKE(0x1DFE4C, 0x0, player_action_encode, packet, action);
}

bool __cdecl player_action_decode(c_bitstream* packet, struct player_action* action)
{
	return INVOKE(0x1E01CB, 0x0, player_action_decode, packet, action);
}

void __cdecl simulation_machine_update_encode(c_bitstream* packet, struct simulation_machine_update* machine_update)
{
	INVOKE(0x1E08E7, 0x0, simulation_machine_update_encode, packet, machine_update);
}

bool __cdecl simulation_machine_update_decode(c_bitstream* packet, struct simulation_machine_update* machine_update)
{
	return INVOKE(0x1E0935, 0x0, simulation_machine_update_decode, packet, machine_update);
}
