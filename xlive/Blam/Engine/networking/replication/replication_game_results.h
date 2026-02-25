#pragma once
#include "game/game_results.h"

/* structures */

struct c_game_results_replicator
{
	class c_simulation_view* m_view;
	bool m_fatal_error;
	bool m_sending_updates;
	bool m_receiving_updates;
	int32 m_update_number;
	s_game_results_incremental_update m_game_results_incremental;
	uint32 m_update_timestamp;
};
ASSERT_STRUCT_SIZE(c_game_results_replicator, 19420);