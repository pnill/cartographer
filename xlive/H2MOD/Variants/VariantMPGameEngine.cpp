
#include "VariantMPGameEngine.h"
#include "H2MOD\Modules\Startup\Startup.h"

std::unordered_map<size_t, c_game_engine_base*> custom_engines;
// returns the custom engine for a given engine pointer
c_game_engine_base *get_custom_engine(size_t thisptr)
{
	if (!LOG_CHECK(custom_engines.find(thisptr) != custom_engines.end()))
	{
		LOG_TRACE_FUNC("Fatal error can't get custom engine");
		std::exit(-1);
	}
	return LOG_CHECK(custom_engines[thisptr]);
}

/* Creates a function prototype for a c_game_engine function, takes name, return type and args */
#define c_engine_func_proto(name, return_type, ...) \
	return_type __fastcall name(c_engine_internal* thisptr, __int32 __EDX__, ##__VA_ARGS__)
/* Creates a function prototype for a c_game_engine function, takes name, return type and args */
#define c_engine_func_proto_ptr(name, return_type, ...) \
	return_type (__fastcall *name)(c_engine_internal* thisptr, __int32 __EDX__, ##__VA_ARGS__)

/*
	V-table for c_game_engine class in game code.
*/
struct c_game_engine_vtable
{
	c_engine_func_proto_ptr(get_engine_id, unsigned int);
	c_engine_func_proto_ptr(setup_scenario, unsigned __int8);
	c_engine_func_proto_ptr(cleanup, void);
	c_engine_func_proto_ptr(unk_function_4, int);
	c_engine_func_proto_ptr(unk_function_5, int, signed int arg1);
	c_engine_func_proto_ptr(unk_function_players_1, void, int arg1);
	c_engine_func_proto_ptr(update_player__maybe__maybe_spawn, void, int arg1);
	c_engine_func_proto_ptr(unk_function_8, void, int arg1);
	c_engine_func_proto_ptr(unk_function_9, void, int arg1);
	c_engine_func_proto_ptr(unk_function_players_2, void, int arg1, int arg2);
	c_engine_func_proto_ptr(unk_function_11, void, int arg1);
	c_engine_func_proto_ptr(unk_function_12, void);
	c_engine_func_proto_ptr(unk_function_13, void);
	c_engine_func_proto_ptr(render_in_world, void, size_t arg1);
	c_engine_func_proto_ptr(update_hud, void, int hud_owner);
	c_engine_func_proto_ptr(unk_function_16, void, int arg1);
	c_engine_func_proto_ptr(unk_function_17, int, int arg1, int arg2);
	c_engine_func_proto_ptr(unk_function_18, int, int arg1, int arg2);
	c_engine_func_proto_ptr(unk_function_19, void);
	c_engine_func_proto_ptr(player_speed_multiplier, float, int arg1);
	c_engine_func_proto_ptr(unk_function_21, int, int arg1);
	c_engine_func_proto_ptr(unk_function_22, void, int arg1);
	c_engine_func_proto_ptr(unk_function_23, void, int arg1);
	c_engine_func_proto_ptr(unk_function_24, void, int arg1, int arg2);
	c_engine_func_proto_ptr(unk_function_25, int, int arg1, int arg2);
	c_engine_func_proto_ptr(unk_function_26, int, signed int arg1, int arg2, signed int arg3);
	c_engine_func_proto_ptr(unk_function_27, int);
	c_engine_func_proto_ptr(is_team_enemy, int, BYTE team_a, BYTE team_b);
	c_engine_func_proto_ptr(unk_function_29, void, int arg1);
	c_engine_func_proto_ptr(unk_function_30, void, signed int arg1, int arg2, signed int arg3);
	c_engine_func_proto_ptr(unk_function_31, void, int arg1, int arg2, char arg3, signed int arg4);
	c_engine_func_proto_ptr(unk_function_32, int, signed int arg1, int arg2, signed int arg3);
	c_engine_func_proto_ptr(unk_function_33, void, int arg1, int arg2);
	c_engine_func_proto_ptr(unk_function_34, int, int arg1, int arg2);
	c_engine_func_proto_ptr(unk_function_35, bool, int arg1);
	c_engine_func_proto_ptr(unk_function_36, int, int arg1, signed int arg2);
	c_engine_func_proto_ptr(unk_function_37, void, int arg1);
	c_engine_func_proto_ptr(unk_function_38, int, int arg1, signed int arg2);
	c_engine_func_proto_ptr(should_garbage_collect, bool, int arg1);
	c_engine_func_proto_ptr(unk_function_40, void);
	c_engine_func_proto_ptr(unk_function_41, void);
	c_engine_func_proto_ptr(unk_function_42, void*, void* arg1);
	c_engine_func_proto_ptr(unk_function_43, void*, char a1, size_t *a2, void *a3);
	c_engine_func_proto_ptr(unk_function_44, bool, char a1, void *a2);
	c_engine_func_proto_ptr(unk_function_45, int, char a1, int a2);
	c_engine_func_proto_ptr(unk_function_46, int, char a1, int a2, int a3);
	c_engine_func_proto_ptr(unk_function_47, bool, char a1, int a2, int a3);
	c_engine_func_proto_ptr(unk_function_48, void*, int a1, int a2, void *a3);
	c_engine_func_proto_ptr(unk_function_49, void, int arg1, int arg2, int arg3, int arg4);
	c_engine_func_proto_ptr(unk_function_50, bool, __int16 a1, __int16 a2, int a3, int a4);
	c_engine_func_proto_ptr(unk_function_51, int, int arg1, int arg2, int arg3, int arg4, int arg5);
};
static_assert(sizeof(c_game_engine_vtable) == 0xCC, "Bad c_game_engine_vtable size");

struct c_engine_internal
{
	c_game_engine_vtable *vtable;
};

c_game_engine_vtable *get_c_slayer_engine()
{
	return Memory::GetAddress<c_game_engine_vtable*>(0x3C2F3C);
}

c_engine_internal ** get_game_mode_engines()
{
	return Memory::GetAddress<c_engine_internal**>(0x4D8548);
}

/*
	Returns base engine set for the current engine or failing that the slayer engine
*/
c_engine_internal *c_game_engine_base::get_base_engine() const
{
	static c_engine_internal backup_engine;
	backup_engine.vtable = get_c_slayer_engine();

	auto engine = get_game_mode_engines()[base_type];
	if (!engine)
		engine = &backup_engine;
	return engine;
}

/* game -> cartographer */
#pragma region c_engine_wrappers

/* helper macro */
#define GET_ID() \
	reinterpret_cast<DWORD>(thisptr)
/* helper macro */
#define GET_ENGINE() \
	(get_custom_engine(GET_ID()))

c_engine_func_proto(get_engine_id_wrapper, unsigned int)
{
	DWORD id = GET_ID();
	if (custom_engines.find(id) != custom_engines.end())
	{
		return static_cast<unsigned int>(get_custom_engine(id)->get_type());
	}
	return -1;
}

c_engine_func_proto(setup_scenario_wrapper, unsigned __int8)
{
	return GET_ENGINE()->setup();
}

c_engine_func_proto(cleanup_wrapper, void)
{
	GET_ENGINE()->cleanup();
}

c_engine_func_proto(unk_function_4_wrapper, int)
{
	return GET_ENGINE()->unk_function_4();
}

c_engine_func_proto(unk_function_5_wrapper, int, signed int arg1)
{
	return GET_ENGINE()->unk_function_5(arg1);
}

c_engine_func_proto(unk_function_players_1_wrapper, void, int arg1)
{
	GET_ENGINE()->unk_function_players_1(arg1);
}

c_engine_func_proto(update_player__maybe__maybe_spawn_wrapper, void, int arg1)
{
	GET_ENGINE()->update_player__maybe__maybe_spawn(arg1);
}

c_engine_func_proto(unk_function_8_wrapper, void, int arg1)
{
	GET_ENGINE()->unk_function_8(arg1);
}

c_engine_func_proto(unk_function_9_wrapper, void, int arg1)
{
	GET_ENGINE()->unk_function_9(arg1);
}
c_engine_func_proto(unk_function_players_2_wrapper, void, int arg1, int arg2)
{
	GET_ENGINE()->unk_function_players_2(arg1, arg2);
}
c_engine_func_proto(unk_function_11_wrapper, void, int arg1)
{
	GET_ENGINE()->unk_function_11(arg1);
}
c_engine_func_proto(unk_function_12_wrapper, void)
{
	GET_ENGINE()->unk_function_12();
}
c_engine_func_proto(unk_function_13_wrapper, void)
{
	GET_ENGINE()->unk_function_12();
}
c_engine_func_proto(unk_function_render_wrapper, void, size_t arg1)
{
	GET_ENGINE()->render_in_world(arg1);
}
c_engine_func_proto(update_hud_wrapper, void, int hud_owner)
{
	GET_ENGINE()->update_hud(hud_owner);
}
c_engine_func_proto(unk_function_16_wrapper, void, int arg1)
{
	GET_ENGINE()->unk_function_16(arg1);
}
c_engine_func_proto(unk_function_17_wrapper, int, int arg1, int arg2)
{
	return GET_ENGINE()->unk_function_17(arg1, arg2);
}
c_engine_func_proto(unk_function_18_wrapper, int, int arg1, int arg2)
{
	return GET_ENGINE()->unk_function_18(arg1, arg2);
}
c_engine_func_proto(unk_function_19_wrapper, void)
{
	GET_ENGINE()->unk_function_19();
}
c_engine_func_proto(update_wrapper, float, int arg1)
{
	return GET_ENGINE()->player_speed_multiplier(arg1);
}
c_engine_func_proto(unk_function_21_wrapper, int, int arg1)
{
	return GET_ENGINE()->unk_function_21(arg1);
}
c_engine_func_proto(unk_function_22_wrapper, void, int arg1)
{
	GET_ENGINE()->unk_function_22(arg1);
}
c_engine_func_proto(unk_function_23_wrapper, void, int arg1)
{
	GET_ENGINE()->unk_function_23(arg1);
}
c_engine_func_proto(unk_function_24_wrapper, void, int arg1, int arg2)
{
	GET_ENGINE()->unk_function_24(arg1, arg2);
}
c_engine_func_proto(unk_function_25_wrapper, int, int arg1, int arg2)
{
	return GET_ENGINE()->unk_function_25(arg1, arg2);
}
c_engine_func_proto(unk_function_26_wrapper, int, signed int arg1, int arg2, signed int arg3)
{
	return GET_ENGINE()->unk_function_26(arg1, arg2, arg3);
}
c_engine_func_proto(unk_function_27_wrapper, int)
{
	return GET_ENGINE()->unk_function_27();
}
c_engine_func_proto(is_team_enemy_wrapper, int, BYTE team_a, BYTE team_b)
{
	return GET_ENGINE()->is_team_enemy(static_cast<e_object_team>(team_a), static_cast<e_object_team>(team_b));
}
c_engine_func_proto(unk_function_29_wrapper, void, int arg1)
{
	GET_ENGINE()->unk_function_29(arg1);
}
c_engine_func_proto(unk_function_30_wrapper, void, signed int arg1, int arg2, signed int arg3)
{
	GET_ENGINE()->unk_function_30(arg1, arg2, arg3);
}
c_engine_func_proto(unk_function_31_wrapper, void, int arg1, int arg2, char arg3, signed int arg4)
{
	GET_ENGINE()->unk_function_31(arg1, arg2, arg3, arg4);
}
c_engine_func_proto(unk_function_32_wrapper, int, signed int arg1, int arg2, signed int arg3)
{
	return GET_ENGINE()->unk_function_32(arg1, arg2, arg3);
}
c_engine_func_proto(unk_function_33_wrapper, void, int arg1, int arg2)
{
	return GET_ENGINE()->unk_function_33(arg1, arg2);
}
c_engine_func_proto(unk_function_34_wrapper, int, int arg1, int arg2)
{
	return GET_ENGINE()->unk_function_34(arg1, arg2);
}
c_engine_func_proto(unk_function_35_wrapper, bool, int arg1)
{
	return GET_ENGINE()->unk_function_35(arg1);
}
c_engine_func_proto(unk_function_36_wrapper, int, int arg1, signed int arg2)
{
	return GET_ENGINE()->unk_function_36(arg1, arg2);
}
c_engine_func_proto(unk_function_37_wrapper, void, int arg1)
{
	return GET_ENGINE()->unk_function_37(arg1);
}
c_engine_func_proto(unk_function_38_wrapper, int, int arg1, signed int arg2)
{
	return GET_ENGINE()->unk_function_38(arg1, arg2);
}
c_engine_func_proto(should_garbage_collect_wrapper, bool, int arg1)
{
	return GET_ENGINE()->should_garbage_collect(arg1);
}
c_engine_func_proto(unk_function_40_wrapper, void)
{
	return GET_ENGINE()->unk_function_40();
}
c_engine_func_proto(unk_function_41_wrapper, void)
{
	return GET_ENGINE()->unk_function_41();
}
c_engine_func_proto(unk_function_42_wrapper, void*, void* arg1)
{
	return GET_ENGINE()->unk_function_42(arg1);
}
c_engine_func_proto(unk_function_43_wrapper, void*, char a1, size_t *a2, void *a3)
{
	return GET_ENGINE()->unk_function_43(a1, a2, a3);
}
c_engine_func_proto(unk_function_44_wrapper, bool, char a1, void *a2)
{
	return GET_ENGINE()->unk_function_44(a1, a2);
}
c_engine_func_proto(unk_function_45_wrapper, int, char a1, int a2)
{
	return GET_ENGINE()->unk_function_45(a1, a2);
}
c_engine_func_proto(unk_function_46_wrapper, int, char a1, int a2, int a3)
{
	return GET_ENGINE()->unk_function_46(a1, a2, a3);
}
c_engine_func_proto(unk_function_47_wrapper, bool, char a1, int a2, int a3)
{
	return GET_ENGINE()->unk_function_47(a1, a2, a3);
}
c_engine_func_proto(unk_function_48_wrapper, void*, int a1, int a2, void *a3)
{
	return GET_ENGINE()->unk_function_48(a1, a2, a3);
}
c_engine_func_proto(unk_function_49_wrapper, void, int arg1, int arg2, int arg3, int arg4)
{
	return GET_ENGINE()->unk_function_49(arg1, arg2, arg3, arg4);
}
c_engine_func_proto(unk_function_50_wrapper, bool, __int16 a1, __int16 a2, int a3, int a4)
{
	return GET_ENGINE()->unk_function_50(a1, a2, a3, a4);
}
c_engine_func_proto(unk_function_51_wrapper, int, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	return GET_ENGINE()->unk_function_51(arg1, arg2, arg3, arg4, arg5);
}

#undef GET_ENGINE
#undef GET_ID
#pragma endregion

/* cartographer -> game */
#pragma region c_base_engine_wrappers
/* Calls a base engine function */
#define call_c_base_func(name, ...) \
	get_base_engine()->vtable->name(get_base_engine(), 0, ##__VA_ARGS__)
bool c_game_engine_base::setup()
{
	return call_c_base_func(setup_scenario);
}
void c_game_engine_base::cleanup()
{
	call_c_base_func(cleanup);
}
int c_game_engine_base::unk_function_4()
{
	return call_c_base_func(unk_function_4);
}
int c_game_engine_base::unk_function_5(signed int arg1)
{
	return call_c_base_func(unk_function_5, arg1);
}

void c_game_engine_base::unk_function_players_1(int arg1)
{
	return call_c_base_func(unk_function_players_1, arg1);
}
void c_game_engine_base::update_player__maybe__maybe_spawn(int arg1)
{
	return call_c_base_func(update_player__maybe__maybe_spawn, arg1);
}
void c_game_engine_base::unk_function_8(int arg1)
{
	return call_c_base_func(unk_function_8, arg1);
}
void c_game_engine_base::unk_function_9(int arg1)
{
	return call_c_base_func(unk_function_9, arg1);
}
void c_game_engine_base::unk_function_players_2(int arg1, int arg2)
{
	return call_c_base_func(unk_function_players_2, arg1, arg2);
}
void c_game_engine_base::unk_function_11(int arg1)
{
	return call_c_base_func(unk_function_11, arg1);
}
void c_game_engine_base::unk_function_12()
{
	return call_c_base_func(unk_function_12);
}
void c_game_engine_base::unk_function_13()
{
	return call_c_base_func(unk_function_13);
}
void c_game_engine_base::render_in_world(size_t arg1)
{
	return call_c_base_func(render_in_world, arg1);
}
void c_game_engine_base::update_hud(int hud_owner)
{
	return call_c_base_func(update_hud, hud_owner);
}
void c_game_engine_base::unk_function_16(int arg1)
{
	return call_c_base_func(unk_function_16, arg1);
}
int c_game_engine_base::unk_function_17(int arg1, int arg2)
{
	return call_c_base_func(unk_function_17, arg1, arg2);
}
int c_game_engine_base::unk_function_18(int arg1, int arg2)
{
	return call_c_base_func(unk_function_18, arg1, arg2);
}
void c_game_engine_base::unk_function_19()
{
	return call_c_base_func(unk_function_19);
}
float c_game_engine_base::player_speed_multiplier(int arg1)
{
	return call_c_base_func(player_speed_multiplier, arg1);
}
int c_game_engine_base::unk_function_21(int arg1)
{
	return call_c_base_func(unk_function_21, arg1);
}
void c_game_engine_base::unk_function_22(int arg1)
{
	return call_c_base_func(unk_function_22, arg1);

}
void c_game_engine_base::unk_function_23(int arg1)
{
	return call_c_base_func(unk_function_23, arg1);
}
void c_game_engine_base::unk_function_24(int arg1, int arg2)
{
	return call_c_base_func(unk_function_24, arg1, arg2);
}
int c_game_engine_base::unk_function_25(int arg1, int arg2)
{
	return call_c_base_func(unk_function_25, arg1, arg2);
}
int c_game_engine_base::unk_function_26(signed int arg1, int arg2, signed int arg3)
{
	return call_c_base_func(unk_function_26, arg1, arg2, arg3);
}
int c_game_engine_base::unk_function_27()
{
	return call_c_base_func(unk_function_27);
}
bool c_game_engine_base::is_team_enemy(e_object_team team_a, e_object_team team_b)
{
	return call_c_base_func(is_team_enemy, team_a, team_b);
}
void c_game_engine_base::unk_function_29(int arg1)
{
	return call_c_base_func(unk_function_29, arg1);
}
void c_game_engine_base::unk_function_30(signed int arg1, int arg2, signed int arg3)
{
	return call_c_base_func(unk_function_30, arg1, arg2, arg3);
}
void c_game_engine_base::unk_function_31(int arg1, int arg2, char arg3, signed int arg4)
{
	return call_c_base_func(unk_function_31, arg1, arg2, arg3, arg4);
}
int c_game_engine_base::unk_function_32(signed int arg1, int arg2, signed int arg3)
{
	return call_c_base_func(unk_function_32, arg1, arg2, arg3);
}
void c_game_engine_base::unk_function_33(int arg1, int arg2)
{
	return call_c_base_func(unk_function_33, arg1, arg2);
}
int c_game_engine_base::unk_function_34(int arg1, int arg2)
{
	return call_c_base_func(unk_function_34, arg1, arg2);
}
bool c_game_engine_base::unk_function_35(int arg1)
{
	return call_c_base_func(unk_function_35, arg1);
}
int c_game_engine_base::unk_function_36(int arg1, signed int arg2)
{
	return call_c_base_func(unk_function_36, arg1, arg2);
}
void c_game_engine_base::unk_function_37(int arg1)
{
	return call_c_base_func(unk_function_37, arg1);
}
int c_game_engine_base::unk_function_38(int arg1, signed int arg2)
{
	return call_c_base_func(unk_function_38, arg1, arg2);
}
bool c_game_engine_base::should_garbage_collect(int arg1)
{
	return call_c_base_func(should_garbage_collect, arg1);
}
void c_game_engine_base::unk_function_40()
{
	return call_c_base_func(unk_function_40);
}
void c_game_engine_base::unk_function_41()
{
	return call_c_base_func(unk_function_41);
}
void* c_game_engine_base::unk_function_42(void* arg1)
{
	return call_c_base_func(unk_function_42, arg1);
}
void* c_game_engine_base::unk_function_43(char a1, size_t *a2, void *a3)
{
	return call_c_base_func(unk_function_43, a1, a2, a3);
}
bool c_game_engine_base::unk_function_44(char a1, void *a2)
{
	return call_c_base_func(unk_function_44, a1, a2);
}
int c_game_engine_base::unk_function_45(char a1, int a2)
{
	return call_c_base_func(unk_function_45, a1, a2);
}
int c_game_engine_base::unk_function_46(char a1, int a2, int a3)
{
	return call_c_base_func(unk_function_46, a1, a2, a3);
}
bool c_game_engine_base::unk_function_47(char a1, int a2, int a3)
{
	return call_c_base_func(unk_function_47, a1, a2, a3);
}
void* c_game_engine_base::unk_function_48(int a1, int a2, void *a3)
{
	return call_c_base_func(unk_function_48, a1, a2, a3);
}
void c_game_engine_base::unk_function_49(int arg1, int arg2, int arg3, int arg4)
{
	return call_c_base_func(unk_function_49, arg1, arg2, arg3, arg4);
}
bool c_game_engine_base::unk_function_50(__int16 a1, __int16 a2, int a3, int a4)
{
	return call_c_base_func(unk_function_50, a1, a2, a3, a4);
}
int c_game_engine_base::unk_function_51(int arg1, int arg2, int arg3, int arg4, int arg5)
{
	return call_c_base_func(unk_function_51, arg1, arg2, arg3, arg4, arg5);
}

#pragma endregion

c_game_engine_vtable custom_engine_wrapper_vtble;

void custom_game_engines::init()
{
	custom_engine_wrapper_vtble.get_engine_id = get_engine_id_wrapper;
	custom_engine_wrapper_vtble.setup_scenario = setup_scenario_wrapper;
	custom_engine_wrapper_vtble.cleanup = cleanup_wrapper;
	custom_engine_wrapper_vtble.unk_function_4 = unk_function_4_wrapper;
	custom_engine_wrapper_vtble.unk_function_5 = unk_function_5_wrapper;
	custom_engine_wrapper_vtble.unk_function_players_1 = unk_function_players_1_wrapper;
	custom_engine_wrapper_vtble.update_player__maybe__maybe_spawn = update_player__maybe__maybe_spawn_wrapper;
	custom_engine_wrapper_vtble.unk_function_8 = unk_function_8_wrapper;
	custom_engine_wrapper_vtble.unk_function_9 = unk_function_9_wrapper;
	custom_engine_wrapper_vtble.unk_function_players_2 = unk_function_players_2_wrapper;
	custom_engine_wrapper_vtble.unk_function_11 = unk_function_11_wrapper;
	custom_engine_wrapper_vtble.unk_function_12 = unk_function_12_wrapper;
	custom_engine_wrapper_vtble.unk_function_13 = unk_function_13_wrapper;
	custom_engine_wrapper_vtble.render_in_world = unk_function_render_wrapper;
	custom_engine_wrapper_vtble.update_hud = update_hud_wrapper;
	custom_engine_wrapper_vtble.unk_function_16 = unk_function_16_wrapper;
	custom_engine_wrapper_vtble.unk_function_17 = unk_function_17_wrapper;
	custom_engine_wrapper_vtble.unk_function_18 = unk_function_18_wrapper;
	custom_engine_wrapper_vtble.unk_function_19 = unk_function_19_wrapper;
	custom_engine_wrapper_vtble.player_speed_multiplier = update_wrapper;
	custom_engine_wrapper_vtble.unk_function_21 = unk_function_21_wrapper;
	custom_engine_wrapper_vtble.unk_function_22 = unk_function_22_wrapper;
	custom_engine_wrapper_vtble.unk_function_23 = unk_function_23_wrapper;
	custom_engine_wrapper_vtble.unk_function_24 = unk_function_24_wrapper;
	custom_engine_wrapper_vtble.unk_function_25 = unk_function_25_wrapper;
	custom_engine_wrapper_vtble.unk_function_26 = unk_function_26_wrapper;
	custom_engine_wrapper_vtble.unk_function_27 = unk_function_27_wrapper;
	custom_engine_wrapper_vtble.is_team_enemy = is_team_enemy_wrapper;
	custom_engine_wrapper_vtble.unk_function_29 = unk_function_29_wrapper;
	custom_engine_wrapper_vtble.unk_function_30 = unk_function_30_wrapper;
	custom_engine_wrapper_vtble.unk_function_31 = unk_function_31_wrapper;
	custom_engine_wrapper_vtble.unk_function_32 = unk_function_32_wrapper;
	custom_engine_wrapper_vtble.unk_function_33 = unk_function_33_wrapper;
	custom_engine_wrapper_vtble.unk_function_34 = unk_function_34_wrapper;
	custom_engine_wrapper_vtble.unk_function_35 = unk_function_35_wrapper;
	custom_engine_wrapper_vtble.unk_function_36 = unk_function_36_wrapper;
	custom_engine_wrapper_vtble.unk_function_37 = unk_function_37_wrapper;
	custom_engine_wrapper_vtble.unk_function_38 = unk_function_38_wrapper;
	custom_engine_wrapper_vtble.should_garbage_collect = should_garbage_collect_wrapper;
	custom_engine_wrapper_vtble.unk_function_40 = unk_function_40_wrapper;
	custom_engine_wrapper_vtble.unk_function_41 = unk_function_41_wrapper;
	custom_engine_wrapper_vtble.unk_function_42 = unk_function_42_wrapper;
	custom_engine_wrapper_vtble.unk_function_43 = unk_function_43_wrapper;
	custom_engine_wrapper_vtble.unk_function_44 = unk_function_44_wrapper;
	custom_engine_wrapper_vtble.unk_function_45 = unk_function_45_wrapper;
	custom_engine_wrapper_vtble.unk_function_46 = unk_function_46_wrapper;
	custom_engine_wrapper_vtble.unk_function_47 = unk_function_47_wrapper;
	custom_engine_wrapper_vtble.unk_function_48 = unk_function_48_wrapper;
	custom_engine_wrapper_vtble.unk_function_49 = unk_function_49_wrapper;
	custom_engine_wrapper_vtble.unk_function_50 = unk_function_50_wrapper;
	custom_engine_wrapper_vtble.unk_function_51 = unk_function_51_wrapper;
}

bool custom_game_engines::register_engine(c_game_engine_types id, c_game_engine_base *engine, c_game_engine_types base_id)
{
	// not freed till process exit
	auto wrapper_class = new c_engine_internal;
	if (!wrapper_class)
		return false;
	wrapper_class->vtable = &custom_engine_wrapper_vtble;

	engine->set_type(id);
	engine->set_base_type(base_id);

	custom_engines[reinterpret_cast<size_t>(wrapper_class)] = engine;

	get_game_mode_engines()[id] = wrapper_class;
	return true;
}
