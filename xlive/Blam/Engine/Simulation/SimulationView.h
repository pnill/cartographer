#pragma once
#include "MachineID.h"

enum e_simulation_view_type : __int16
{
	_simulation_view_type_none = 0x0,
	_simulation_view_type_synchronous_to_remote_authority = 0x1,
	_simulation_view_type_synchronous_to_remote_client = 0x2,
	_simulation_view_type_distributed_to_remote_authority = 0x3,
	_simulation_view_type_distributed_to_remote_client = 0x4,
	k_simulation_view_type_count = 0x5,
};

enum e_simulation_view_death_reason
{
	_simulation_view_death_reason_none = 0x0,
	_simulation_view_death_reason_disconnected = 0x1,
	_simulation_view_death_reason_out_of_sync = 0x2,
	_simulation_view_death_reason_failed_to_join = 0x3,
	_simulation_view_death_reason_blocking = 0x4,
	_simulation_view_death_reason_catchup_fail = 0x5,
	_simulation_view_death_reason_ended = 0x6,
	_simulation_view_death_reason_mode_error = 0x7,
	_simulation_view_death_reason_player_error = 0x8,
	_simulation_view_death_reason_replication_entity = 0x9,
	_simulation_view_death_reason_replication_event = 0xA,
	_simulation_view_death_reason_replication_game_results = 0xB,
	k_simulation_view_reason_count = 0xC,
};

#pragma pack(push,1)
struct c_simulation_view
{
	char field_0[2];
	e_simulation_view_type m_view_type;
	datum view_datum_index;
	DWORD m_distributed_view;
	void* m_world;
	unsigned int m_view_index;
	s_machine_identifier machine_id;
	char gap_1A[2];
	int m_peer_index;
	void* m_observer;
	unsigned int m_observer_channel_index;
	e_simulation_view_death_reason m_view_death_reason;
	DWORD m_view_establishment_mode;
	DWORD field_30;
	signed int field_34;
	unsigned int field_38;
	unsigned int m_channel_index;
	unsigned int field_40;
	void* field_44;
	signed int field_48;
	signed int field_4C;
	signed int field_50;
	signed int field_54;
	signed int field_58;
	signed int field_5C;
	signed int field_60;
	signed int field_64;
	signed int field_68;
	signed int field_6C;
	signed int field_70;
	char gap_74;
	char field_74;
	char field_76[2];
	char field_78[4];
	signed int field_7C;
	unsigned int field_80;
	unsigned int field_84;
	char field_88[8];
	signed int field_90;
	LPVOID m_synchronous_catchup_buffer;
	signed int field_98;
	int field_9C;
	signed int field_A0;
	char gap_A4[8];
	signed int field_AC;
	signed int field_B0;
};
#pragma pack(pop)
CHECK_STRUCT_SIZE(c_simulation_view, 0xB4);
