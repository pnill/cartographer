#pragma once
#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"
#include "Blam/Common/Common.h"
#include "Blam/Engine/Networking/NetworkPrimitiveTypes.h"
#include "Blam/Engine/Game/memory/bitstream.h"
#include <wtypes.h>

enum e_simulation_entity_type : __int16
{
	_simulation_entity_type_none = -1,
	_simulation_entity_type_slayer_engine_globals = 0x0,
	_simulation_entity_type_ctf_engine_globals = 0x1,
	_simulation_entity_type_oddball_engine_globals = 0x2,
	_simulation_entity_type_king_engine_globals = 0x3,
	_simulation_entity_type_territories_engine_globals = 0x4,
	_simulation_entity_type_juggernaut_engine_globals = 0x5,
	_simulation_entity_type_game_engine_player = 0x6,
	_simulation_entity_type_game_engine_statborg = 0x7,
	_simulation_entity_type_breakable_surface_group = 0x8,
	_simulation_entity_type_unit = 0x9,
	_simulation_entity_type_item = 0xA,
	_simulation_entity_type_generic = 0xB,
	_simulation_entity_type_vehicle = 0xC,
	_simulation_entity_type_projectile = 0xD,
	_simulation_entity_type_weapon = 0xE,
	_simulation_entity_type_turret = 0xF,
	_simulation_entity_type_device = 0x10,
	k_simulation_entity_count = 0x11,
};

struct s_simulation_game_entity
{
	DWORD entity_index;
	e_simulation_entity_type entity_type;
	bool exists_in_gameworld;
	char event_reference_count;
	datum object_index;
	signed int entity_update_flag;
	signed int field_10;
	int creation_data_size;
	void* creation_data;
	int state_data_size;
	void* state_data;
};
CHECK_STRUCT_SIZE(s_simulation_game_entity, 0x24);

class  c_simulation_entity_definition
{
public:
	virtual  e_simulation_entity_type entity_type();
	virtual  const char* entity_type_name();
	virtual  DWORD state_data_size();
	virtual  DWORD creation_data_size();
	virtual  signed int update_flag_count();
	virtual  DWORD initial_update_mask();
	virtual  char sub_A90039(int a1);
	virtual  char sub_A90043(int a1);
	virtual  char sub_A90048(int a1, int a2);
	virtual  int  sub_A9004D(int a1, int a2, DWORD a3);
	virtual  void get_creation_relevance_string(int a1, int a2, int a3, int a4);
	virtual  int calculate_update_relevance(int a1, int a2, DWORD* a3);
	virtual  void write_creation_to_packet(DWORD creation_data_size, void* creation_data, void* telementary_data, bitstream* packet);
	virtual  bool read_creation_from_packet(DWORD creation_data_size, void* creation_data, bitstream* packet);
	virtual  bool write_update_to_packet(bool unk, DWORD update_mask, DWORD* update_mask_written, DWORD state_data_size, void* state_data, void* telementary_data, bitstream* packet, int required_leave_space_bits);
	virtual  bool read_update_from_packet(bool a1, DWORD* out_update_mask, DWORD state_data_size, void* state_data, bitstream* packet);
	virtual  bool sub_A90891(int a1, int a2, int a3);
	virtual  char sub_A908DB(int a1, int a2, int a3);
	virtual  unsigned int build_creation_data(s_simulation_game_entity* entity, DWORD creation_data_size, void* out_creation_data);
	virtual  bool build_baseline_state_data(DWORD creation_data_size, void* creation_data, DWORD state_data_size, void* baseline_state_data);
	virtual  bool build_updated_state_data(s_simulation_game_entity* entity, DWORD* update_mask, DWORD state_data_size, void* state_data);
	virtual  unsigned int sub_A9026A(int a1);
	virtual  bool create_game_entity(s_simulation_game_entity* entity, DWORD creation_data_size, void* creation_data, DWORD mask, DWORD state_data_size, void* state_data);
	virtual  bool update_game_entity(s_simulation_game_entity* entity, DWORD update_flags, DWORD state_data_size, void* state_data);
	virtual  bool delete_game_entity(s_simulation_game_entity* entity);
	virtual  bool promote_game_entity_to_authority(s_simulation_game_entity* entity);

	//there were more but this much was enough
};
