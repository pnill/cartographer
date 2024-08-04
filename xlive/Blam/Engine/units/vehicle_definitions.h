#pragma once
#include "unit_definitions.h"
#include "havok/physics/collide/shape/compund/list/hkListShape.h"
#include "havok/physics/collide/shape/multishpere/hkMultiSphere.h"

#define k_vehicle_gear_count_max = 16
#define k_maximum_anti_gravity_points_per_vehicle_physics_definition = 16
#define k_maximum_friction_points_per_vehicle_physics_definition = 16

enum e_vehicle_definition_flags : uint32
{
	_vehicle_definition_speed_wakes_physics = FLAG(0),
	_vehicle_definition_turn_wakes_physics = FLAG(1),
	_vehicle_definition_driver_power_wakes_physics = FLAG(2),
	_vehicle_definition_gunner_power_wakes_physics = FLAG(3),
	_vehicle_definition_control_opposite_speed_sets_brake = FLAG(4),
	_vehicle_definition_slide_wakes_physics = FLAG(5),
	_vehicle_definition_kills_riders_at_terminal_velocity = FLAG(6),
	_vehicle_definition_causes_collision_damage = FLAG(7),
	_vehicle_definition_ai_weapon_cannot_rotate = FLAG(8),
	_vehicle_definition_ai_does_not_require_driver = FLAG(9),
	_vehicle_definition_ai_unused = FLAG(10),
	_vehicle_definition_ai_driver_enable = FLAG(11),
	_vehicle_definition_ai_driver_flying = FLAG(12),
	_vehicle_definition_ai_driver_cansidestep = FLAG(13),
	_vehicle_definition_ai_driver_hovering = FLAG(14),
	_vehicle_definition_vehicle_steers_directly = FLAG(15),
	_vehicle_definition_unused = FLAG(16),
	_vehicle_definition_has_ebrake = FLAG(17),
	_vehicle_definition_noncombat_vehicle = FLAG(18),
	_vehicle_definition_no_friction_wdriver = FLAG(19),
	_vehicle_definition_can_trigger_automatic_opening_doors = FLAG(20),
	_vehicle_definition_autoaim_when_teamless = FLAG(21),
};

enum e_vehicle_definition_type : uint16
{
	_vehicle_definition_type_human_tank = 0,
	_vehicle_definition_type_human_jeep = 1,
	_vehicle_definition_type_human_boat = 2,
	_vehicle_definition_type_human_plane = 3,
	_vehicle_definition_type_alien_scout = 4,
	_vehicle_definition_type_alien_fighter = 5,
	_vehicle_definition_type_turret = 6,
};

enum e_vehicle_definition_control : uint16
{
	_vehicle_definition_control_normal = 0,
	_vehicle_definition_control_unused = 1,
	_vehicle_definition_control_tank = 2,
};

enum e_vehicle_definition_specific_type : uint16
{
	_vehicle_definition_specific_type_none = 0,
	_vehicle_definition_specific_type_ghost = 1,
	_vehicle_definition_specific_type_wraith = 2,
	_vehicle_definition_specific_type_spectre = 3,
	_vehicle_definition_specific_type_sentinel_enforcer = 4
};

enum e_vehicle_definition_player_training_type : uint16
{
	_vehicle_definition_player_training_none = 0,
	_vehicle_definition_player_training_warthog = 1,
	_vehicle_definition_player_training_warthog_turret = 2,
	_vehicle_definition_player_training_ghost = 3,
	_vehicle_definition_player_training_banshee = 4,
	_vehicle_definition_player_training_tank = 5,
	_vehicle_definition_player_training_wraith = 6
};

enum e_vehicle_definition_vehicle_size : uint16
{
	_vehicle_definition_size_small = 0,
	_vehicle_definition_size_large = 1,
};

enum e_havok_vehicle_physics_definition_flags : uint32
{
	_havok_vehicle_physics_definition_invalid = FLAG(0)
};

enum e_anti_gravity_point_definition_flags : uint32
{
	_anti_gravity_point_definition_gets_damage_from_regions = FLAG(0)
};

enum e_friction_point_definition : uint32
{
	_friction_point_definition_gets_damage_from_region = FLAG(0),
	_friction_point_definition_powered = FLAG(1),
	_friction_point_definition_front_turning = FLAG(2),
	_friction_point_definition_rear_turning = FLAG(3),
	_friction_point_definition_attached_to_ebrake = FLAG(4),
	_friction_point_definition_can_be_destroyed = FLAG(5),
};

enum e_friction_point_definition_friction_type : uint16
{
	_friction_point_definition_friction_point = 0,
	_friction_point_definition_friction_forward = 1,
};

enum e_friction_point_definition_model_state : uint16
{
	_friction_point_definition_model_state_default = 0,
	_friction_point_definition_model_state_minor_damage = 1,
	_friction_point_definition_model_state_medium_damage = 2,
	_friction_point_definition_model_state_major_damage = 3,
	_friction_point_definition_model_state_destroyed = 4
};

// max: 1
struct s_vehicle_phantom_shape_definition
{
	hkListShape<4> shapes;

	uint32 multi_sphere_count;
	uint32 flags;
	int32 pad[2];
	real_rectangle3d bounds;

	hkMultiSphereShape multi_spheres[4];
};
ASSERT_STRUCT_SIZE(s_vehicle_phantom_shape_definition, 672);

// max: k_maximum_friction_points_per_vehicle_physics_definition
struct s_friction_point_definition
{
	string_id marker_name;

	e_friction_point_definition flags;
	/// (0.0-1.0) fraction of total vehicle mass
	real32 fraction_of_mass;
	real32 radius;
	/// radius when the tire is blown off.
	real32 damaged_radius;
	e_friction_point_definition_friction_type friction_type;
	int16 pad_1;

	real32 moving_friction_velocity_difference;
	real32 ebrake_moving_friction;
	real32 ebrake_friction;
	real32 ebrake_moving_friction_velocity_difference;

	int8 pad_2[20];

	string_id collision_global_material_name;

	int16 pad_3;

	// Explaination("friction point destruction data", "")
	/// only need point can destroy flag set
	e_friction_point_definition_model_state model_state;
	/// only need point can destroy flag set
	string_id region_name;

	int32 pad_4;
};
ASSERT_STRUCT_SIZE(s_friction_point_definition, 76);

// max: k_maximum_anti_gravity_points_per_vehicle_physics_definition
struct s_anti_gravity_point_definition
{
	string_id marker_name;

	e_anti_gravity_point_definition_flags flags;

	real32 anti_gravity_strength;
	real32 anti_gravity_offset;
	real32 anti_gravity_height;
	real32 anti_gravity_damp_factor;
	real32 anti_gravity_normal_K1;
	real32 anti_gravity_normal_K0;
	real32 radius;

	int8 pad_1[16];
	string_id damage_source_region_name;

	real32 default_state_error;
	real32 minor_damage_error;
	real32 medium_damage_error;
	real32 major_damage_error;
	real32 destroyed_state_error;
};
ASSERT_STRUCT_SIZE(s_anti_gravity_point_definition, 76);

struct s_havok_vehicle_physics_definition
{
	e_havok_vehicle_physics_definition_flags flags;
	///  for friction based vehicles only
	real32 ground_friction;
	///  for friction based vehicles only
	real32 ground_depth;
	///  for friction based vehicles only
	real32 ground_damp_factor;
	///  for friction based vehicles only
	real32 ground_moving_friction;
	/// degrees 0-90
	real32 ground_maximum_slope_0;
	/// degrees 0-90.  and greater than slope 0
	real32 ground_maximum_slope_1;
	int8 pad_1[16];
	/// lift per WU.
	real32 anit_gravity_bank_lift;
	/// how quickly we bank when we steer 
	real32 steering_bank_reaction_scale;
	/// value of 0 defaults to 1.  .5 is half gravity
	real32 gravity_scale;
	/// generated from the radius of the hkConvexShape for this vehicle
	real32 radius;

	tag_block<s_anti_gravity_point_definition> anti_gravity_points;

	tag_block<s_friction_point_definition> friction_points;

	tag_block<s_vehicle_phantom_shape_definition> phantom_shapes;
};
ASSERT_STRUCT_SIZE(s_havok_vehicle_physics_definition, 84);

struct s_torque_curve
{
	real_bounds torque;
	real32 peak_torque_scale;
	real32 past_peak_torque_exponent;
	/// generally 0 for loading torque and something less than max torque for cruising torque
	real32 torque_at_max_angular_velocity;
	real32 torque_at_2x_max_angular_velocity;
};

// max: k_vehicle_gear_count_max
struct s_vehicle_gear
{
	// Explaination("loaded torque", "")
	s_torque_curve loaded_torque_curve;
	// Explaination("cruising torque", "")
	s_torque_curve cruising_torque_curve;

	// Explaination("gearing", "")
	/// seconds
	real32 min_time_to_upshift;
	/// 0-1
	real32 engine_up_shift_scale;
	real32 gear_ratio;
	/// seconds
	real32 min_time_to_down_shift;
	/// 0-1
	real32 engine_down_shift_scale;
};


struct _vehicle_definition
{
	_unit_definition unit;

	// Explaination("$$$ VEHICLE $$$", "")
	e_vehicle_definition_flags flags;
	e_vehicle_definition_type type;
	e_vehicle_definition_control control;

	real32 maximum_forward_speed;
	real32 maximum_reverse_speed;

	real32 speed_acceleration;
	real32 speed_deceleration;

	real32 maximum_left_turn;
	/// use negative values
	real32 maximum_right_turn;

	real32 wheel_circumference;
	real32 turn_rate;
	real32 wheel_blur_speed;

	/// if your type corresponds to something in this list choose it
	e_vehicle_definition_specific_type specific_type;

	e_vehicle_definition_player_training_type player_training_vehicle_type;

	string_id flip_message;

	real32 turn_scale;
	real32 speed_turn_penalty_power;
	real32 speed_turn_penalty;

	real32 maximum_left_slide;
	real32 maximum_right_slide;

	real32 slide_acceleration;
	real32 slide_deceleration;

	real_bounds flipping_angular_velocity;

	/// The size determine what kind of seats in larger vehicles it may occupy (i.e. small or large cargo seats)
	e_vehicle_definition_vehicle_size vehicle_size;

	int16 pad_1;

	real_euler_angles2d fixed_gun_angles;

	// Explaination("steering overdampening", "when the steering is off by more than the cusp angle
	//		the steering will overcompensate more and more.when it
	//		is less, it overcompensates less and less.the exponent
	//		should be something in the neighborhood of 2.0")
	real_angle over_dampen_cusp_angle;
	real32 over_dampen_exponent;

	real32 crouch_transition_time_seconds;

	int32 pad_2;
	// Explaination("engine", "")
	/// higher moments make engine spin up slower
	real32 engine_moment;
	/// higher moments make engine spin up slower
	real32 engine_max_angular_velocity;

	tag_block<s_vehicle_gear> gears;

	real32 flying_torque_scale;
	real32 seat_entrance_acceleration_scale;
	real32 seat_exit_acceleration_scale;
	real32 air_friction_deceleration;
	real32 thrust_scale;

	tag_reference suspension_sound;
	tag_reference crash_sound;
	tag_reference unused;
	tag_reference special_effect;
	tag_reference unused_effect;

	s_havok_vehicle_physics_definition havok_vehicle_physics;
};
ASSERT_STRUCT_SIZE(_vehicle_definition, 768);

void __cdecl vehicle_definitions_fixup(datum vehicle_datum);