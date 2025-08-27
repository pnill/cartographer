#pragma once
#include "simulation_game_entities.h"
#include "objects/emblems.h"
#include "objects/object_placement.h"

struct s_simulation_object_state_data
{
	real_point3d position;
	real_vector3d forward;
	real_vector3d up;
	real32 scale;
	real_vector3d translational_velocity;
	real_vector3d angular_velocity;
	real32 body_vitality;
	char body_stun_ticks_is_zero;
	char gap45[3];
	real32 shield_vitality;
	char shield_stun_ticks_is_zero;
	char dead;
	char gap4D[2];
	char region_count;
	char region_states[16];
	uint8 constraint_count;
	__int16 destroyed_constraints;
	__int16 loosened_constraints;
	char pad[42];
};
ASSERT_STRUCT_SIZE(s_simulation_object_state_data, 0x90);

struct s_simulation_object_creation_data
{
	datum object_scenario_datum_index;
	datum object_definition_index;
	int8 multiplayer_spawn_monitor_index;
	// unused padding
	char pad[3];

	s_emblem_info emblem_info;
	
	// Repurpose padding for variant index
	int8 model_variant_index;
};
ASSERT_STRUCT_SIZE(s_simulation_object_creation_data, 16);

class c_simulation_game_object_entity_definition : public c_simulation_entity_definition
{
public:
	virtual int8 entity_replication_required_for_view_activation(s_simulation_game_entity* entity) override;
	virtual int8 get_object_index(s_simulation_game_entity* entity) override;
	virtual bool build_baseline_state_data(int32 creation_data_size, void* creation_data, int32 state_data_size, void* out_state_baseline_data) override;
	virtual uint32 rotate_entity_indices(s_simulation_game_entity* entity) override;
	virtual bool update_game_entity(s_simulation_game_entity* entity, uint32 update_flags, int32 state_data_size, void* state_data) override;
	virtual bool delete_game_entity(s_simulation_game_entity* entity) override;
	virtual bool promote_game_entity_to_authority(s_simulation_game_entity* entity) override;
	virtual uint32 build_creation_data(s_simulation_game_entity* entity, int32 creation_data_size, void* out_creation_data) override;

	// c_simulation_game_object_entity_definition additions

	virtual void* handle_object_update(datum object_index, uint32 update_mask, int32 state_data_size, void* state_data) = 0;
	virtual void* object_required_to_join_game(datum object_index) = 0;
	virtual void* create_object(int32 creation_data_size, void* creation_data, uint32* flags, int32 internal_state_data_size, void* initial_state_data) = 0;
	virtual void* unk31(int32 a1) = 0;
	virtual void* apply_object_update(datum object_index, uint32 entity_sync_flag, int32 state_data_size, const void* state_data) = 0;
	virtual void* promote_object_to_authority(datum object_index) = 0;
	virtual void* ensure_object_position_update_quantization_inside_bsp(void) = 0;
	virtual void* object_interpolation_data_out(int32 a1, void* a2, void* a3) = 0;
	virtual void* object_interpolation_data(int32 a1, void* a2, void* a3) = 0;
};


datum simulation_object_get_replicated_object_from_entity(int32 entity_index);

bool __stdcall c_simulation_object_entity_definition__object_setup_placement_data(
	void* _this,
	s_simulation_object_creation_data* object_creation_data,
	s_simulation_object_state_data* state_data,
	uint32* flags,
	object_placement_data* placement_data);

datum __stdcall c_simulation_object_entity_definition__object_create_object(void* _this,
	s_simulation_object_creation_data* object_creation,
	s_simulation_object_state_data* state_data,
	uint32* flags,
	object_placement_data* placement_data);

void simulation_game_objects_apply_patches(void);
