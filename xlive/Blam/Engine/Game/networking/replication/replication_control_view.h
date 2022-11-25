#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Engine/Game/math/real_math.h"
#include <wtypes.h>

struct s_player_predicted_aim_assist
{
	float primary_auto_aim_level;
	float secondary_auto_aim_level;
	DWORD entity_index;
	unsigned int model_target;
};
CHECK_STRUCT_SIZE(s_player_predicted_aim_assist, 0x10);

struct s_player_motion
{
	real_euler_angles2d facing;
	real_point2d throttle;
	WORD motion_flags;
	WORD weapon_set_identifier;
	BYTE primary_weapon_index;
	BYTE secondary_weapon_index;
	WORD zoom_level;
	bool primary_predicted_fire_primary;
	bool primary_predicted_fire_secondary;
	bool secondary_predicted_fire_primary;
	bool secondary_predicted_fire_secondary;
	bool predicted_aim_assist_exists;
	BYTE gap_1D[3];
	s_player_predicted_aim_assist aim_info;
	bool weapon_fire_empty_primary;
	bool weapon_fire_empty_secondary;
	bool motion_sensor_velocity_threshold_exceeded;
	BYTE gap33;
};
CHECK_STRUCT_SIZE(s_player_motion, 0x34);

struct s_player_prediction
{
	DWORD predicted_entity_index;
	real_vector3d point_quantized;
	real_vector3d prediction_axis;
	real_vector3d prediction_axis2;
	real_point3d prediction_translation_velocity;
	real_point3d prediction_angular_velocity;
};
CHECK_STRUCT_SIZE(s_player_prediction, 0x40);

enum e_network_memory_block : __int16
{
	_network_memory_block_message_outgoing = 0x0,
	_network_memory_block_message_incoming = 0x1,
	_network_memory_block_replication_entity_status = 0x2,
	_network_memory_block_replication_entity_update = 0x3,
	_network_memory_block_replication_entity_packet = 0x4,
	_network_memory_block_replication_event_header = 0x5,
	_network_memory_block_replication_event_payload = 0x6,
	_network_memory_block_replication_event_record = 0x7,
	_network_memory_block_replication_event_payload2 = 0x8,
	_network_memory_block_replication_control_motion = 0x9,
	_network_memory_block_replication_control_prediction = 0xA,
	_network_memory_block_simulation_synchronous_client = 0xB,
	_network_memory_block_simulation_entity_creation = 0xC,
	_network_memory_block_simulation_entity_state = 0xD,
	_network_memory_block_simulation_event = 0xE,
	_network_memory_block_logic_session_array = 0xF,
	_network_memory_block_logic_unsuitable_session_array = 0x10,
	_network_memory_block_join_request = 0x11,
	k_network_memory_block_count = 0x12,
};

struct s_replication_allocation_block
{
	unsigned __int16 block_size;
	e_network_memory_block block_type;
	void* block_data;
};
CHECK_STRUCT_SIZE(s_replication_allocation_block, 0x8);

struct s_replication_control_request
{
	DWORD unknown_count;
	unsigned int control_index;
	char gap_8[52];
	signed int block_count;
	s_replication_allocation_block blocks[2];
	char gap_50[48];
};

struct c_replication_control_view
{
	char gap_0[4];
	DWORD field_4;
	char gap_8[4];
	DWORD unk_vtable;
	DWORD motion_flags;
	DWORD field_14;
	char gap_18[1792];
	DWORD player_prediction_flags;
	DWORD field_71C;
	s_player_prediction player_prediction[16];
};
CHECK_STRUCT_SIZE(c_replication_control_view, 0xB20);
