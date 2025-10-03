#include "stdafx.h"
#include "ai.h"

#include "actors.h"
#include "actor_dynamic_firing_position.h"
#ifdef AI_DEBUG
#include "ai_debug.h"
#endif
#include "ai_flocks.h"
#include "ai_orders.h"
#include "ai_dialogue.h"
#include "ai_mission_dialogue.h"
#include "ai_player.h"
#include "ai_script.h"
#include "clumps.h"
#include "cl_engine.h"
#include "gravemind.h"
#include "joint_behavior.h"
#include "props.h"
#include "squads.h"
#include "swarms.h"

#include "game/game.h"

/* prototypes */

static ai_globals_type* ai_globals_get(void);

/* public code */

void ai_apply_patches(void)
{
    // Replace function so we can initialize ai in MP
    WritePointer((uintptr_t)&get_game_systems()[59].initialize_for_new_map_proc, ai_initialize_for_new_map);
    return;
}

void __cdecl ai_update(void)
{
	INVOKE(0x30D941, 0x2B86B8, ai_update);
	return;
}

void __cdecl ai_initialize_for_new_map(void)
{
    ai_globals_type* ai_globals = ai_globals_get();

    ai_players_initialize_for_new_map();

#ifdef AI_DEBUG
    ai_debug_initialize_for_new_map();
#endif
    actors_initialize_for_new_map();
    swarms_initialize_for_new_map();
    props_initialize_for_new_map();
    
    //nullsub();
    
    squads_initialize_for_new_map();
    orders_initialize_for_new_map();
    clumps_initialize_for_new_map();
    joint_behavior_initialize_for_new_map();
    dynamic_firing_sets_initialize_for_new_map();
    cs_initialize_for_new_map();
    ai_script_initialize_for_new_map();
    ai_dialogue_initialize_for_new_map();
    ai_scenes_initialize_for_new_map();
    flocks_initialize_for_new_map();
    gravemind_initialize_for_new_map();

    ai_globals->spatial_effects_last_index = 0;
    ai_globals->spatial_effects_first_index = 0;
    csmemset(ai_globals->spatial_effects, 0, sizeof(ai_globals->spatial_effects));
    ai_globals->ai_initialized_for_map = true;
	return;
}

/* private code */

static ai_globals_type* ai_globals_get(void)
{
    return *Memory::GetAddress<ai_globals_type**>(0xA965CC, 0x9A1C4C);
}
