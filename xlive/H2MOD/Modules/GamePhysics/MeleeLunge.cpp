#include "stdafx.h"

#include "MeleeLunge.h"

#include "Util\Memory.h"

#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"

#include <float.h>
#if (!defined(_M_FP_FAST)) || !_M_FP_FAST
#pragma fenv_access (on)
#endif

bool melee_lunge_hook_enabled = true;

#define MELEE_DEBUG 0

#if MELEE_DEBUG
#define LOG_TRACE_MELEE(msg, ...) \
 ((void) ((LOG_TRACE_GAME(msg, __VA_ARGS__), true))) \

#else
#define LOG_TRACE_MELEE(msg, ...) ((void)0)
#endif

#if _DEBUG
#pragma optimize( "", on )
#endif
float __cdecl get_ticks_difference_real()
{
	auto time_globals = time_globals::get();
	return (float)time_globals->ticks_per_second / 30.f;
}
#if _DEBUG
#pragma optimize( "", off )
#endif

float get_melee_acceleration(float max_speed_per_tick)
{
	return max_speed_per_tick / 3.0f;
}

// not entirely sure what this calculates
float melee_lunge_compute_something_1(float v1, float acceleration)
{
	return ((v1 - acceleration) * 3.0f) / 2.0f;
}

// added in h3, needed for melee velocity pinning
void object_get_localized_velocity(datum object_index, real_vector3d* translational_velocity, real_vector3d* angular_velocity)
{
	typedef bool(__cdecl* object_get_early_mover_local_space_velocity)(datum a1, real_vector3d *translational_velocity, char not_force_get_localized_velocity);
	auto p_object_get_early_mover_local_space_velocity = Memory::GetAddressRelative<object_get_early_mover_local_space_velocity>(0x54C17C);

	typedef void(__cdecl* object_get_velocities)(datum object_indee, real_vector3d* translational_velocity, real_vector3d* angular_velocity);
	auto p_object_get_velocities = Memory::GetAddressRelative<object_get_velocities>(0x532BDE);

	real_vector3d early_mover_velocity = {};

	p_object_get_velocities(object_index, translational_velocity, angular_velocity);
	if (p_object_get_early_mover_local_space_velocity(object_index, &early_mover_velocity, false))
	{
		if (translational_velocity)
			*translational_velocity = *translational_velocity - early_mover_velocity;

		if (angular_velocity)
			*angular_velocity = *angular_velocity - early_mover_velocity;
	}
}

void __cdecl biped_dash_hook(datum object_index, datum target_player, char weapon_is_sword)
{
	auto p_biped_dash = Memory::GetAddressRelative<void(__cdecl*)(datum, datum, bool)>(0x5576A9);

	p_biped_dash(object_index, target_player, weapon_is_sword);

	BYTE* object_data = (BYTE*)object_get_fast_unsafe(object_index);

	// check if we actually entered melee mode, otherwise don't update the data
	if (*(BYTE*)(object_data + 1012) == 6)
	{
		c_character_physics_mode_melee_datum* melee_mode_datum = (c_character_physics_mode_melee_datum*)(object_data + 1012 + 16);

		real_vector3d localized_velocity;
		object_get_localized_velocity(object_index, &localized_velocity, nullptr);

		// store the localized velocity in the melee physics mode datum
		melee_mode_datum->field_1C = localized_velocity;

		// limit the vector's length
		limit3d(&melee_mode_datum->field_1C, 1.5f);
	}
}

int __cdecl biped_dash_time_to_target(datum biped_index)
{
	BYTE* object_data = (BYTE*)object_get_fast_unsafe(biped_index);

	// check if we actually entered melee mode, otherwise don't update the data
	if (*(BYTE*)(object_data + 1012) == 6)
	{

	}

	return NONE;
}

float __cdecl get_max_melee_lunge_speed_per_tick(float target_distance, bool weapon_is_sword)
{
	float max_velocity = 8.0f;
	if (weapon_is_sword)
		max_velocity = 12.0f;

	float t1 = target_distance * 0.375f;
	if (t1 >= k_valid_real_epsilon)
		return blam_min(time_globals::get_seconds_per_tick() * max_velocity, t1);
	else
		return k_valid_real_epsilon;
}

/*
NOTES:

	** At higher tickrates than 30 (at least double or higher), the lunge update executes twice or more ticks compared with 30 tick
	** And in theory it should move the player a shorter distance per tick over the same distance
*/

/*
	outputs new flags
*/

#define FINISH_DECELERATION(flags) \
	if ((flags) > 0) { \
		out_current_flags |= (flags); \
	} \
	this->m_time_to_target_in_ticks = 0; \
	physics_output->translational_velocity = *current_velocity; \

void c_character_physics_mode_melee_datum::melee_deceleration_fixup
(
	s_character_physics_output* physics_output,
	real_point3d* object_origin,
	real_vector3d* current_velocity,
	real_vector3d* aiming_vector,
	BYTE& out_current_flags,
	BYTE& out_deceleration_ticks
)
{
	if (!this->m_has_target)
		return;

	/*if (this->m_weapon_is_sword)
	{
		BYTE* biped_data = (BYTE*)this - 0x10 - 0x3F4;

		BYTE* animation_data = biped_data + *(WORD*)(biped_data + 0x35A);
		BYTE melee_animation_tick = *(BYTE*)(animation_data + 0x32);
		BYTE melee_animation_action_tick = *(BYTE*)(animation_data + 0x33);

		LOG_TRACE_MELEE("{} - melee animation tick: {}, melee action tick index: {}, melee max counter: {}",
			__FUNCTION__,
			melee_animation_tick,
			melee_animation_action_tick,
			this->m_maximum_counter);
	}*/

	/*if (this->m_melee_tick == 1)
	{
		int added_ticks = (this->m_maximum_counter - 6) - (this->m_weapon_is_sword ? 7 : 1);
	}*/

	real_vector3d target_vector = this->m_target_point - *object_origin;
	float remaining_distance_from_player_position = magnitude3d(&target_vector);
	float max_speed_per_tick = get_max_melee_lunge_speed_per_tick(m_distance, m_weapon_is_sword);

	float current_velocity_and_aiming_vector_dot_product = dot_product3d(&m_aiming_direction, current_velocity);
	current_velocity_and_aiming_vector_dot_product *= time_globals::get_seconds_per_tick();

	if (this->m_started_decelerating)
	{
		// preserve default output by original code
		real_vector3d default_output = physics_output->translational_velocity;

		// copy current_velocity in output_velocity
		physics_output->translational_velocity = *current_velocity;

		// get the velocity per game logic update
		physics_output->translational_velocity = physics_output->translational_velocity * time_globals::get()->seconds_per_tick;

		real_vector3d direction = physics_output->translational_velocity;

		// get the direction of the melee lunge and the magnitude as well
		float normalized_current_magnitude_per_tick = normalize3d(&direction);

		// this variable is named max_speed_per_tick_2 because it should result in the same value after processing in `compute something`
		// because passing both arguments with the same value will cause that
		float max_speed_per_tick_2 = melee_lunge_compute_something_1(max_speed_per_tick, get_melee_acceleration(max_speed_per_tick));

		float real_time_to_target = ((remaining_distance_from_player_position / max_speed_per_tick_2) - 0.5f);

		// field_28 is always the same after the first melee tick
		float unk1 = m_field_28 * 1.5;
		double unk2 = 0.75;
		if (unk1 >= 0.75)
		{
			if (unk1 <= 3.5)
				unk2 = unk1;
			else
				unk2 = 3.5;
		}

		float maybe_minimum_velocity = time_globals::get_seconds_per_tick() * unk2;

		float current_velocity_per_tick = magnitude3d(&physics_output->translational_velocity);

		//float decelerated_velocity = (float)out_deceleration_ticks * velocity_to_decelerate_per_tick;

		// TODO: this does not work perfectly
		// do some hack calculations to compute minimum velocity
		/*float initial_velocity_before_deceleration = this->m_velocity_to_decelerate + unk3;
		float minimum_velocity_per_tick = initial_velocity_before_deceleration - this->m_velocity_to_decelerate;
		if (unk3 > initial_velocity_before_deceleration - unk3)
			minimum_velocity_per_tick = 0.0f;*/

		//const float melee_max_cosine = cos(degreesToRadians(85.f));
		const float melee_max_cosine = 0.087155744f;

		if (get_max_melee_lunge_speed_per_tick(this->m_distance, this->m_weapon_is_sword) <= k_valid_real_epsilon)
		{
			FINISH_DECELERATION(FLAG(melee_decelration_unk_flag));
			return;
		}

		/*LOG_TRACE_MELEE("{} - current velocity/tick: {}, unk3: {}, minimum velocity/tick: {}, velocity to decelerate/tick: {}",
			__FUNCTION__,
			current_velocity_per_tick,
			unk3,
			minimum_velocity_per_tick,
			velocity_to_decelerate_per_tick);

		LOG_TRACE_MELEE("{} - initial velocity before deceleration: {}",
			__FUNCTION__,
			initial_velocity_before_deceleration);*/

		//current_velocity_per_tick > minimum_velocity_per_tick
		//decelerated_velocity > this->velocity_to_decelerate

		/*float aiming_and_player_velocity_product = current_velocity->dot_product(this->aiming_direction);
		aiming_and_player_velocity_product *= time_globals::get_seconds_per_tick();*/

		/*float unk5 = compute_something(aiming_and_player_velocity_product, max_speed_per_tick);
		if (unk5 < 0.0f)
			unk5 = 0.0f;*/

		//float distance_to_target_point = distance_vector.magnitude();

		int ticks_to_add = 0;

		/*if (m_maximum_counter - (m_melee_tick - 1) > k_deceleration_ticks)
		{
			m_maximum_counter = (m_melee_tick - 1) + k_deceleration_ticks;
		}*/

		if (m_maximum_counter + ticks_to_add <= (m_melee_tick - 1))
			out_current_flags |= FLAG(melee_deceleration_finished);

		if (!TEST_FLAG(out_current_flags, melee_deceleration_finished))
		{
			// compare 2 dot products
			if (dot_product3d(aiming_vector, &direction) <= (current_velocity_per_tick * melee_max_cosine) 
				|| normalized_current_magnitude_per_tick == 0.0f)
			{
				float unk4 = (this->m_velocity_to_decelerate + maybe_minimum_velocity) / 3.0f;
				float deceleration = current_velocity_per_tick;
				if (unk4 <= current_velocity_per_tick)
					deceleration = unk4;

				// deceleration *= time_globals::get_ticks_difference_real();

				// im not entirely sure if this is needed or not
				//output->out_translational_velocity = output->out_translational_velocity * time_globals::get_ticks_difference_real();

				physics_output->translational_velocity = (direction * (-deceleration)) + physics_output->translational_velocity;

				// this shit apparently is not converted back to a vector that
				// - defines the velocity in units per seconds intead of units per tick
				// either some numb nut at bungie forgot to add the line bellow or it was intentional
				// so we just convert the current velocity and deceleration to 30 tick values
				// and output them like that
				// output->out_translational_velocity = output->out_translational_velocity * (float)time_globals::get()->ticks_per_second;

				if (maybe_minimum_velocity <= ((current_velocity_per_tick - unk4) + k_valid_real_epsilon))
				{
					return;
				}

				out_current_flags |= FLAG(melee_decelration_unk_flag);
				this->m_time_to_target_in_ticks = 0;
				return;
			}
			else
			{
				if (current_velocity_per_tick <= k_valid_real_epsilon)
				{
					FINISH_DECELERATION(FLAG(melee_decelration_unk_flag));
					return;
				}
			
				//this->m_time_to_target_in_ticks = (int)real_time_to_target;

				float deceleration = this->m_velocity_to_decelerate / k_deceleration_ticks_real;

				this->m_time_to_target_in_ticks = (current_velocity_per_tick / deceleration) - 0.5f;
				
				if (deceleration > current_velocity_per_tick)
				{
					deceleration = current_velocity_per_tick;
				}
				
				physics_output->translational_velocity = (direction * (-deceleration)) + physics_output->translational_velocity;

				//this->m_time_to_target_in_ticks = ((_FP5 * 0.5f) + v124);
				//addDebugText("remaining target distance ticks: %f, %d", ((_FP5 * 0.5f) + v124), this->m_time_to_target_in_ticks);

				//out_deceleration_ticks++;


				//addDebugText("remaining target distance ticks h2 code: %f, %d", ((remaining_distance / something1) - 0.5f), this->m_time_to_target_in_ticks);

				physics_output->translational_velocity = physics_output->translational_velocity * (float)time_globals::get()->ticks_per_second;
				return;
			}
		}

		physics_output->translational_velocity = *current_velocity;
		this->m_time_to_target_in_ticks = 0;
		return;
	}
}

#undef FINISH_DECELERATION

bool c_character_physics_mode_melee_datum::pin_localized_velocity(real_vector3d* output, real_vector3d* localized_velocity)
{
	float output_velocity_magnitude = magnitude3d(output);
	float localized_velocity_magnitude = magnitude3d(localized_velocity) + 0.80000001f;

	bool unk_bool = false;

	if (output_velocity_magnitude > localized_velocity_magnitude)
	{
		unk_bool = true;
		*output = *output * (localized_velocity_magnitude / output_velocity_magnitude);
	}

	real_vector3d new_vec = *localized_velocity - *output;

	float magnitude = normalize3d(&new_vec);
	if (magnitude > 1.2f)
	{
		float dot_product = dot_product3d(output, localized_velocity);
		float unk2 = blam_min(magnitude - 1.2f, -dot_product);
		if (unk2 > 0.0f)
		{
			unk_bool = true;
			*output = (new_vec * unk2) + *output;
		}
	}

	if (output->k - localized_velocity->k <= 0.30000001f)
		return unk_bool;

	output->k = localized_velocity->k + 0.30000001f;

	return true;
}

void __thiscall c_character_physics_mode_melee_datum::update_internal
(
	s_character_physics_output* physics_output,
	bool a3,
	float distance_world_units,
	real_vector3d *target_translational_velocity,
	real_point3d *target_origin,
	real_vector3d *aiming_vector,
	real_vector3d *translational_velocity,
	real_point3d *object_origin
)
{
	//unsigned int csr = _mm_getcsr();
	//unsigned int ctrl87Old = _control87(0, 0);
	//LOG_TRACE_GAME("{} function called with: control87: {:x}, csr: {:x}", __FUNCTION__, ctrl87, csr);

	//_control87(_RC_UP, _MCW_RC);
	//static bool logOnce = false;
	//if (!logOnce)
	//{
		//addDebugText("_control87() 0x%08x", _control87(0, 0) & _MCW_RC);
		//logOnce = true;
	//}

	LOG_TRACE_MELEE("{} : start melee log @ tick {}", __FUNCTION__, m_melee_tick);

	real_vector3d* out_velocity = &physics_output->translational_velocity;

	// we keep some flags in m_started_decelerationg (first 4 bits)
	BYTE melee_flags = this->m_started_decelerating & melee_flags_mask;
	this->m_started_decelerating &= ~melee_flags_mask;

	auto p_character_physics_mode_melee_datum_update_internal = Memory::GetAddressRelative<void(__thiscall*)(c_character_physics_mode_melee_datum*, s_character_physics_output*, char, float, real_vector3d*, real_point3d*, real_vector3d*, real_vector3d*, real_point3d*)>(0x50B0D1, 0x4FD701);
	p_character_physics_mode_melee_datum_update_internal(this, physics_output, a3, distance_world_units, target_translational_velocity, target_origin, aiming_vector, translational_velocity, object_origin);

	if (m_melee_tick == 1)
	{
		int added_ticks = m_maximum_counter - 6 - (m_weapon_is_sword ? 7 : 1);

		LOG_TRACE_MELEE("{} - update melee at tickrate: {}", __FUNCTION__, time_globals::get()->ticks_per_second);
		LOG_TRACE_MELEE("{} - added tick count to maximum counter: {}", __FUNCTION__, added_ticks);
		LOG_TRACE_MELEE("{} - target_distance: {}, maximum counter: {}", __FUNCTION__, distance_world_units, m_maximum_counter);
	}

	// if deceleration has started and max accel tick count - melee tick is bellow 0, it means 4 deceleration ticks have been executed
	BYTE melee_deceleration_tick_count = 0;

	if (time_globals::get()->ticks_per_second > 30 || melee_lunge_hook_enabled == true)
	{
		if (m_started_decelerating)
		{
			melee_deceleration_fixup(physics_output, object_origin, translational_velocity, aiming_vector, melee_flags, melee_deceleration_tick_count);

			if (distance_world_units > 5.0f
				|| TEST_FLAG(melee_flags, melee_decelration_unk_flag)
				|| m_time_to_target_in_ticks <= 0
				|| m_melee_tick >= (m_maximum_counter + 6)
				)
			{
				//pin_localized_velocity(&a2->out_translational_velocity, &localized_velocity);
				physics_output->flags |= FLAG(2); // set to let the engine know we should get out of the melee lunge physics
			}
			else
			{
				physics_output->flags &= ~FLAG(2); // maintain lunge physics
			}
		}
	}

	// this is set to localized_velocity, from biped_dash_hook
	//real_vector3d localized_velocity = field_1C;

	// create vector from subtracting 1 point out of the other 
	real_vector3d distance_vector = m_target_point - *object_origin;
	float remaining_distance = magnitude3d(&distance_vector);

	float unk_float_distance = dot_product3d(&m_aiming_direction, translational_velocity);
	unk_float_distance *= time_globals::get_seconds_per_tick();
	float unk_velocity = melee_lunge_compute_something_1(unk_float_distance, get_max_melee_lunge_speed_per_tick(this->m_distance, this->m_weapon_is_sword));
	if (unk_velocity < 0.0f)
		unk_velocity = 0.0f;

	float log_magnitude = magnitude3d(&physics_output->translational_velocity);

	LOG_TRACE_MELEE("{} : output velocity:       i: {}, j: {}, k: {}, magnitude: {}, decelerating?: {}, remaining distance to target: {}",
		__FUNCTION__,
		out_velocity->i,
		out_velocity->j,
		out_velocity->k,
		log_magnitude,
		this->m_started_decelerating,
		remaining_distance);

	/*LOG_TRACE_MELEE("{} : unk float value: {}, remaining distance: {}",
		__FUNCTION__,
		unk_velocity,
		remaining_distance);*/

	LOG_TRACE_MELEE("{} : remaining distance in ticks: {} ", __FUNCTION__, this->m_time_to_target_in_ticks);

	LOG_TRACE_MELEE("{} : target point: x: {} y: {}, z: {}",
		__FUNCTION__,
		this->m_target_point.x,
		this->m_target_point.y,
		this->m_target_point.z);
	
	LOG_TRACE_MELEE("{} : aiming vector adjusted length: {}",
		__FUNCTION__,
		magnitude3d(&m_aiming_direction));

	LOG_TRACE_MELEE("{} : previous velocity: {}, previous velocity dot product with aiming vector adjusted: {}",
		__FUNCTION__,
		magnitude3d(translational_velocity),
		dot_product3d(&m_aiming_direction, translational_velocity));

	if ((float)(m_maximum_counter - m_melee_tick) <= k_deceleration_ticks_real && !m_started_decelerating)
		LOG_TRACE_MELEE("{} : we are about to start decelerating @ next tick : {}", __FUNCTION__, m_melee_tick);

	LOG_TRACE_MELEE("{} : end log for melee @ tick {} \n", __FUNCTION__, m_melee_tick - 1);

	//csr = _mm_getcsr();
	//ctrl87 = _control87(0, 0);

	//LOG_TRACE_GAME("{} after melee tick with: control87: {:x}, csr: {:x}", __FUNCTION__, ctrl87, csr);

	// update with the new flags returned by h3 algorithm
	this->m_started_decelerating |= melee_flags;

	//_control87(ctrl87Old, 0xFFFFFFFF);

	//field_1C = localized_velocity;
}

__declspec(naked) void call_character_melee_physics_input_update_internal() { __asm jmp c_character_physics_mode_melee_datum::update_internal }

// TODO: 
void c_character_physics_mode_melee_datum::build_initial_melee_parameters(bool valid)
{
}

void c_character_physics_mode_melee_datum::update_melee_parameters()
{
}

void c_character_physics_mode_melee_datum::update_melee_deceleration()
{
}

// check if this is what this actually does
float melee_lunge_get_tick_count(float distance, float max_speed_per_tick)
{
	float f1 = (distance - ((max_speed_per_tick * 4.0f) / 2.0f)) 
		- (((max_speed_per_tick - (max_speed_per_tick / 3.0f)) * 3.0f) / 2.0f); // this might be the acceleration

	f1 = blam_max(f1, 0.0f);

	if (max_speed_per_tick > k_valid_real_epsilon)
		return (f1 / max_speed_per_tick) + 6.0f;
	else
		return 6.0f;
}

void __thiscall c_character_physics_mode_melee_datum::update_internal_2
(
	s_character_physics_output* physics_output,
	bool a3,
	float distance_between_havok_components,
	real_vector3d* target_translational_velocity,
	real_point3d* target_origin,
	real_vector3d* aiming_vector,
	real_vector3d* translational_velocity,
	real_point3d* player_origin
)
{
	LOG_TRACE_MELEE("{} : start melee log @ tick {}", __FUNCTION__, m_melee_tick);
	
	auto p_character_physics_mode_melee_datum_update_internal = Memory::GetAddressRelative<void(__thiscall*)(c_character_physics_mode_melee_datum*, s_character_physics_output*, char, float, real_vector3d*, real_point3d*, real_vector3d*, real_vector3d*, real_point3d*)>(0x50B0D1, 0x4FD701);
	if (!melee_lunge_hook_enabled)
	{
		LOG_TRACE_MELEE("{} : using default halo 2 function!", __FUNCTION__);
		p_character_physics_mode_melee_datum_update_internal(this, physics_output, a3, distance_between_havok_components, target_translational_velocity, target_origin, aiming_vector, translational_velocity, player_origin);
	}
	else
	{
		// to note this will not force exit if the actual animation didn't finish
		bool force_leave_melee_lunge_physics = false;

		if (a3)
		{
			if (!m_has_target && distance_between_havok_components > k_valid_real_epsilon)
			{
				float distance;
				if (m_maximum_distance == 0.0f)
					distance = distance_between_havok_components;
				else
					distance = blam_min(distance_between_havok_components, m_maximum_distance);

				float initial_max_speed_per_tick = get_max_melee_lunge_speed_per_tick(distance, m_weapon_is_sword);

				// this might not actually be melee_lunge_get_tick_count
				float initial_lunge_tick_count = melee_lunge_get_tick_count(distance, initial_max_speed_per_tick);

				m_field_28 = dot_product3d(aiming_vector, target_translational_velocity);

				real_vector3d vector_to_target;
				vector_from_points3d(target_origin, player_origin, &vector_to_target);

				float unk1 = m_field_28 * 1.5f;
				unk1 = blam_min(blam_max(unk1, 0.0f), 2.5f);

				float distance_to_target = magnitude3d(&vector_to_target);
				if (distance_to_target > k_valid_real_epsilon)
				{
					scale_vector3d(&vector_to_target, (distance_between_havok_components / distance_to_target), &vector_to_target);
				}

				scale_vector3d(target_translational_velocity, time_globals::get_seconds_per_tick() * initial_lunge_tick_count * (unk1 / 2.5f), &field_1C);

				real_vector3d unk_vector1;
				add_vectors3d(&field_1C, &vector_to_target, &unk_vector1);

				float new_distance = 0.0f;
				float unk_distance1 = magnitude3d(&unk_vector1);

				if (m_maximum_distance == 0.0f)
					new_distance = unk_distance1;
				else
					new_distance = blam_min(unk_distance1, m_maximum_distance);

				float max_speed_per_tick_2 = get_max_melee_lunge_speed_per_tick(new_distance, m_weapon_is_sword);
				float lunge_tick_count_2 = melee_lunge_get_tick_count(new_distance, max_speed_per_tick_2);

				field_E = m_maximum_distance != 0.0f && unk_distance1 > m_maximum_distance;
				m_distance = field_E ? m_maximum_distance : unk_distance1;

				m_melee_start_origin = *player_origin;
				m_has_target = true;
				m_maximum_counter = m_weapon_is_sword ? 7 : 1;
				m_maximum_counter += (int)lunge_tick_count_2;

				if (unk_distance1 > k_valid_real_epsilon)
				{
					point_from_line3d(player_origin, &unk_vector1, m_distance / unk_distance1, &m_target_point);
				}
				else
				{
					m_target_point = *player_origin;
				}

				vector_from_points3d(&m_target_point, player_origin, &m_aiming_direction);
				if (normalize3d(&m_aiming_direction) == 0.0f)
					m_aiming_direction = *aiming_vector;
			}

			if (m_has_target)
			{
				// update aiming vectors??
				if (!field_E)
				{
					point_from_line3d(player_origin, &m_aiming_direction, distance_between_havok_components, &m_target_point);
					point_from_line3d(&m_target_point, &m_aiming_direction, blam_min(blam_max(0.0f, m_field_28), 2.5f) / 5.0f, &m_target_point);
				}

				vector_from_points3d(&m_target_point, player_origin, &m_aiming_direction);
				if (normalize3d(&m_aiming_direction) == 0.0f)
					m_aiming_direction = *aiming_vector;
			}

			if (dot_product3d(&m_aiming_direction, aiming_vector) < 0.86602539f)
				m_aiming_direction = *aiming_vector;
		}

		physics_output->translational_velocity = *translational_velocity;

		if (m_has_target)
		{
			// initialize some data
			float max_speed_per_tick = get_max_melee_lunge_speed_per_tick(m_distance, m_weapon_is_sword);
			float acceleration = get_melee_acceleration(max_speed_per_tick);

			float aiming_direction_translational_veloctity_product_per_tick = dot_product3d(&m_aiming_direction, translational_velocity) * time_globals::get_seconds_per_tick();

			float unk1 = melee_lunge_compute_something_1(max_speed_per_tick, acceleration);
			float unk2 = blam_max(0.0f, melee_lunge_compute_something_1(aiming_direction_translational_veloctity_product_per_tick, acceleration));

			real_vector3d target_point_vector;
			vector_from_points3d(&m_target_point, player_origin, &target_point_vector);
			float distance_to_target_point = magnitude3d(&target_point_vector);

			real_vector3d current_translational_velocity_per_tick = m_melee_tick == 0 ? global_zero_vector3d : *translational_velocity;
			scale_vector3d(&current_translational_velocity_per_tick, time_globals::get_seconds_per_tick(), &current_translational_velocity_per_tick);

			// check if we should start decelerating
			if (max_speed_per_tick > k_valid_real_epsilon)
			{
				if (m_started_decelerating
					|| unk2 > distance_to_target_point
					|| (float)(m_maximum_counter - m_melee_tick) <= k_deceleration_ticks_real)
				{
					// deceleration code
					// field_28 is always the same after the first melee tick
					float unk3 = m_field_28 * 1.5f;

					// not entirely sure if this is actually min_velocity_after_deceleration_per_tick
					// but it looks like it
					double temp = blam_max(blam_min(unk3, 3.5), 0.75);

					float min_velocity_after_deceleration_per_tick = time_globals::get_seconds_per_tick() * temp;

					// magnitude3d might change the floating point unit settings
					// apparently the issue is present in default code as well
					float temp_current_velocity_per_tick = magnitude3d(&current_translational_velocity_per_tick);
					
					real_vector3d direction_of_current_translational_velocity = current_translational_velocity_per_tick;
					float current_velocity_per_tick_from_normalization = normalize3d(&direction_of_current_translational_velocity);

					if (!m_started_decelerating)
					{
						// if we weren't previously decelerating
						// build the deceleration parameters
						m_started_decelerating = true;
						m_velocity_to_decelerate = blam_max(min_velocity_after_deceleration_per_tick, temp_current_velocity_per_tick - min_velocity_after_deceleration_per_tick);
						m_distance_to_target_point_before_deceleration = blam_max(distance_to_target_point, k_valid_real_epsilon);
					}

					if (dot_product3d(aiming_vector, &direction_of_current_translational_velocity) <= (temp_current_velocity_per_tick * 0.087155744f)
						|| current_velocity_per_tick_from_normalization == 0.0f)
					{
						float deceleration = (this->m_velocity_to_decelerate + min_velocity_after_deceleration_per_tick) / 3.0f;
						deceleration = blam_min(deceleration, temp_current_velocity_per_tick);

						point_from_line3d(&current_translational_velocity_per_tick, &direction_of_current_translational_velocity, -deceleration, &physics_output->translational_velocity);

						// for some reason in the actual game the following line of code is missing from the actual game
						// no idea why
						//scale_vector3d(&physics_output->out_translational_velocity, (float)time_globals::seconds_to_ticks_precise(1.0f), &physics_output->out_translational_velocity);

						if (min_velocity_after_deceleration_per_tick > (temp_current_velocity_per_tick - ((m_velocity_to_decelerate + min_velocity_after_deceleration_per_tick) / 3.0f) + k_valid_real_epsilon))
						{
							force_leave_melee_lunge_physics = true;
							m_time_to_target_in_ticks = 0;
						}
					}
					else
					{
						if (temp_current_velocity_per_tick > k_valid_real_epsilon)
						{
							float deceleration = blam_min(m_velocity_to_decelerate / k_deceleration_ticks_real, temp_current_velocity_per_tick);
							point_from_line3d(&current_translational_velocity_per_tick, &direction_of_current_translational_velocity, -deceleration, &physics_output->translational_velocity);
							scale_vector3d(&physics_output->translational_velocity, (float)time_globals::seconds_to_ticks_real(1.0f), &physics_output->translational_velocity);
							m_time_to_target_in_ticks = (int)((distance_to_target_point / unk1) - 0.5f);
							
							// this does "fix" (more like improve, it still doesn't match 30 tick behaviour) the sword going too hard 
							// but breaks the sword glitches like butterflying and sword flying
							// float remaining_time_to_target_real = floor(((current_velocity_per_tick_from_normalization / deceleration) - 0.5f));
							// remaining_time_to_target_real += (remaining_time_to_target_real > 0.0f ? 0.5f : -0.5f);
							// m_time_to_target_in_ticks = (int)remaining_time_to_target_real;
						}
						else
						{
							force_leave_melee_lunge_physics = true;
							m_time_to_target_in_ticks = 0;
						}
					}
				}
				else if ((float)m_melee_tick >= 5.0f)
				{
					// maintain current velocity
					physics_output->translational_velocity = current_translational_velocity_per_tick;
					scale_vector3d(&physics_output->translational_velocity, (float)time_globals::seconds_to_ticks_real(1.0f), &physics_output->translational_velocity);
					m_time_to_target_in_ticks = (int)((((distance_to_target_point - unk1) / max_speed_per_tick) + 3.0f) - 0.5f);
				}
				else
				{
					// acceleration
					// TODO cleanup
					float acceleration_1 = max_speed_per_tick - blam_max(0.0f, aiming_direction_translational_veloctity_product_per_tick);
					float acceleration_2 = blam_max(0.0f, acceleration_1);

					float final_acceleration = blam_min(get_melee_acceleration(max_speed_per_tick), acceleration_2);

					// float final_acceleration = 0.0f;
					// if (acceleration_2 >= get_melee_acceleration(max_speed_per_tick))
					// 	final_acceleration = get_melee_acceleration(max_speed_per_tick);
					// else if (acceleration_1 >= 0.0f)
					// 	final_acceleration = max_speed_per_tick;

					point_from_line3d(&current_translational_velocity_per_tick, &m_aiming_direction, final_acceleration, &physics_output->translational_velocity);
					scale_vector3d(&physics_output->translational_velocity, (float)time_globals::seconds_to_ticks_real(1.0f), &physics_output->translational_velocity);

					// static_assert(offsetof(c_character_physics_mode_melee_datum, m_aiming_direction) == 0x3C);

					float time_to_target = ((((
						(distance_to_target_point - (((((3.0f - (float)m_melee_tick) - 0.5f) * max_speed_per_tick) * 0.5f) + unk1))
						/ max_speed_per_tick)
						+ (3.0f - (float)m_melee_tick))
						+ 3.0f)
						+ 0.5f);

					m_time_to_target_in_ticks = (int)time_to_target;

					LOG_TRACE_MELEE("{} - max_speed_per_tick: {}, acceleration: {}, output magnitude: {}",
						__FUNCTION__, max_speed_per_tick, final_acceleration, magnitude3d(&physics_output->translational_velocity));
				}
			}
			else
			{
				m_started_decelerating = true;
				force_leave_melee_lunge_physics = true;
				m_time_to_target_in_ticks = 0;
			}
		}

		// build_initial_melee_parameters(a3);
		// update_melee_parameters();

		m_melee_tick++;
		if (distance_between_havok_components > 5.0f
			|| force_leave_melee_lunge_physics
			|| m_time_to_target_in_ticks <= 0
			|| m_melee_tick >= (m_maximum_counter + 6)
			)
		{
			//pin_localized_velocity(&a2->out_translational_velocity, &localized_velocity);
			physics_output->flags |= FLAG(2); // set to let the engine know we should get out of the melee lunge physics
		}
		else
		{
			physics_output->flags &= ~FLAG(2); // maintain lunge physics
		}
	}

	if (m_melee_tick == 1)
	{
		int added_ticks = m_maximum_counter - 6 - (m_weapon_is_sword ? 7 : 1);

		LOG_TRACE_MELEE("{} - update melee at tickrate: {}", __FUNCTION__, time_globals::get()->ticks_per_second);
		LOG_TRACE_MELEE("{} - added tick count to maximum counter: {}", __FUNCTION__, added_ticks);
		LOG_TRACE_MELEE("{} - target_distance: {}, maximum counter: {}", __FUNCTION__, distance_between_havok_components, m_maximum_counter);
	}

	real_vector3d vector_to_target;
	vector_from_points3d(&m_target_point, player_origin, &vector_to_target);
	float remaining_distance = magnitude3d(&vector_to_target);

	float unk_float_distance = dot_product3d(&m_aiming_direction, translational_velocity);
	unk_float_distance *= time_globals::get_seconds_per_tick();
	float unk_velocity = melee_lunge_compute_something_1(unk_float_distance, get_max_melee_lunge_speed_per_tick(this->m_distance, this->m_weapon_is_sword));
	if (unk_velocity < 0.0f)
		unk_velocity = 0.0f;

	float log_magnitude = magnitude3d(&physics_output->translational_velocity);

	LOG_TRACE_MELEE("{} : output velocity:       i: {}, j: {}, k: {}, magnitude: {}, decelerating?: {}, remaining distance to target: {}",
		__FUNCTION__,
		physics_output->translational_velocity.i,
		physics_output->translational_velocity.j,
		physics_output->translational_velocity.k,
		log_magnitude,
		m_started_decelerating,
		remaining_distance);

	/*LOG_TRACE_MELEE("{} : unk float value: {}, remaining distance: {}",
		__FUNCTION__,
		unk_velocity,
		remaining_distance);*/

	LOG_TRACE_MELEE("{} : remaining distance in ticks: {} ", __FUNCTION__, m_time_to_target_in_ticks);

	LOG_TRACE_MELEE("{} : target point: x: {} y: {}, z: {}",
		__FUNCTION__,
		this->m_target_point.x,
		this->m_target_point.y,
		this->m_target_point.z);

	LOG_TRACE_MELEE("{} : aiming vector adjusted length (should be 1.0 or a little lower): {}",
		__FUNCTION__,
		magnitude3d(&m_aiming_direction));

	LOG_TRACE_MELEE("{} : previous velocity: {}, previous velocity dot product with aiming vector adjusted: {}",
		__FUNCTION__,
		magnitude3d(translational_velocity),
		dot_product3d(&m_aiming_direction, translational_velocity));

	if ((float)(m_maximum_counter - m_melee_tick) <= k_deceleration_ticks_real && !m_started_decelerating)
		LOG_TRACE_MELEE("{} : we are about to start decelerating @ next tick : {}", __FUNCTION__, m_melee_tick);

	LOG_TRACE_MELEE("{} : end log for melee @ tick {} \n", __FUNCTION__, m_melee_tick - 1);
}

__declspec(naked) void call_character_melee_physics_input_update_internal_2() { __asm jmp c_character_physics_mode_melee_datum::update_internal_2 }

// LOGS
/*

***** 30 tickrate SWORD max target distance

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 0
c_character_physics_mode_melee_datum::update_internal - added tick count: 3
c_character_physics_mode_melee_datum::update_internal - target_distance: 2.7646084, melee_counter: 16
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 0, melee action tick index: 255, melee max counter: 16
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -1.1318847, j: 3.8365142, k: 0, decelerating?: false, magnitude: 4.0000005, remaining distance to target: 2.7646084
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 11
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.23458236 j: -0.31450152, k: -8.49
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 0

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 1
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 1, melee action tick index: 255, melee max counter: 16
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -2.2637694, j: 7.6730285, k: -4.7683716e-07, decelerating?: false, magnitude: 8.000001, remaining distance to target: 2.4980626
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 9
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.27227765 j: -0.4422691, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 1

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 2
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 2, melee action tick index: 255, melee max counter: 16
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -3.3956537, j: 11.509543, k: -4.7683716e-07, decelerating?: false, magnitude: 12.000003, remaining distance to target: 2.2314224
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 8
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.27227014 j: -0.4422438, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 2

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 3
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 3, melee action tick index: 255, melee max counter: 16
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -3.3956537, j: 11.509544, k: -4.7683716e-07, decelerating?: false, magnitude: 12.000003, remaining distance to target: 1.8314812
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 7
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.27225287 j: -0.4421872, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 3

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 4
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 4, melee action tick index: 255, melee max counter: 16
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -3.3956537, j: 11.509544, k: -4.7683716e-07, decelerating?: false, magnitude: 12.000003, remaining distance to target: 1.4315845
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 5
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.2722244 j: -0.442088, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 4

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 5
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 5, melee action tick index: 255, melee max counter: 16
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -3.3956537, j: 11.509544, k: -4.7683716e-07, decelerating?: false, magnitude: 12.000003, remaining distance to target: 1.0317891
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 4
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.27216652 j: -0.44189173, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 5

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 6
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 0, melee action tick index: 3, melee max counter: 16
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -3.3956537, j: 11.509544, k: -4.7683716e-07, decelerating?: false, magnitude: 12.000003, remaining distance to target: 0.6322921
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 3
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.27202412 j: -0.4414091, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 6

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 7
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 1, melee action tick index: 3, melee max counter: 16
c_character_physics_mode_melee_datum::melee_deceleration_fixup - current velocity: 0.40000013 >= min_velocity: 0.025000002
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -2.5997972, j: 8.8119955, k: -3.6507848e-07, decelerating?: true, magnitude: 9.187503, remaining distance to target: 0.23415169
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 0
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.27149788 j: -0.43962544, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 7

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 8
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 2, melee action tick index: 3, melee max counter: 16
c_character_physics_mode_melee_datum::melee_deceleration_fixup - current velocity: 0.30625013 >= min_velocity: 0.025000002
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -1.8039411, j: 6.1144466, k: -2.5331977e-07, decelerating?: true, magnitude: 6.375003, remaining distance to target: 0.06264523
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 0
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.23336934 j: -0.31038895, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 8

// these 2 deceleration ticks are missing at 60 tick (or higher)
c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 9
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 3, melee action tick index: 3, melee max counter: 16
c_character_physics_mode_melee_datum::melee_deceleration_fixup - current velocity: 0.1061288 >= min_velocity: 0.025000002
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -0.10508454, j: 0.35618508, k: 3.5188577e-07, decelerating?: true, magnitude: 0.37136313, remaining distance to target: 0.0007751883
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 0
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.26417547 j: -0.41480565, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 9

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 10
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 4, melee action tick index: 3, melee max counter: 16
c_character_physics_mode_melee_datum::melee_deceleration_fixup - current velocity: 0.012378772 >= min_velocity: 0.025000002
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: 6.9849193e-09, j: -2.7939677e-08, k: 0, decelerating?: true, magnitude: 2.879956e-08, remaining distance to target: 0.007374888
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 0
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.25880522 j: -0.39660284, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 10

***** 30 tickrate SWORD max target distance log end

***** 60 tickrate SWORD max target distance
- broken behaviour, velocity is maintained even after the biped has left melee physics mode, because deceleration kicks in too late by 2 ticks

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 0
c_character_physics_mode_melee_datum::update_internal - added tick count: 10
c_character_physics_mode_melee_datum::update_internal - target_distance: 2.7734096, melee_counter: 23
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 0, melee action tick index: 255, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -3.4894848, j: 1.9553769, k: 5.5018168e-06, decelerating?: false, magnitude: 4.0000005, remaining distance to target: 2.7734094
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 18
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.6067975 j: -1.5594548, k: -8.489996
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 0

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 1
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 1, melee action tick index: 255, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -6.97897, j: 3.9107537, k: 1.1501485e-05, decelerating?: false, magnitude: 8.000001, remaining distance to target: 2.6401858
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 16
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5487351 j: -1.5919907, k: -8.487495
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 1

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 2
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 2, melee action tick index: 255, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.468454, j: 5.86613, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000001, remaining distance to target: 2.506863
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 15
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5487442 j: -1.5919855, k: -8.487495
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 2

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 3
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 3, melee action tick index: 255, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.468455, j: 5.8661304, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000001, remaining distance to target: 2.3068824
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 14
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5487611 j: -1.591976, k: -8.487495
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 3

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 4
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 4, melee action tick index: 255, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.468456, j: 5.866131, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000003, remaining distance to target: 2.1069057
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 12
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5487816 j: -1.5919646, k: -8.487495
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 4

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 5
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 5, melee action tick index: 255, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.468457, j: 5.866131, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000004, remaining distance to target: 1.9069359
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 11
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.548808 j: -1.5919499, k: -8.487495
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 5

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 6
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 6, melee action tick index: 255, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.468458, j: 5.866131, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000005, remaining distance to target: 1.7069752
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 10
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5488424 j: -1.5919307, k: -8.487495
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 6

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 7
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 7, melee action tick index: 255, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.468459, j: 5.866131, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000005, remaining distance to target: 1.5070275
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 9
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.548888 j: -1.5919052, k: -8.487495
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 7

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 8
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 0, melee action tick index: 6, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.46846, j: 5.866131, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000007, remaining distance to target: 1.3071003
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 8
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5489516 j: -1.5918697, k: -8.487496
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 8

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 9
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 1, melee action tick index: 6, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.468461, j: 5.866131, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000007, remaining distance to target: 1.1072054
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 7
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5490433 j: -1.5918183, k: -8.487496
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 9

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 10
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 2, melee action tick index: 6, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.468462, j: 5.866131, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000008, remaining distance to target: 0.9073638
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 6
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5491817 j: -1.591741, k: -8.487497
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 10

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 11
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 3, melee action tick index: 6, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.468462, j: 5.866131, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000008, remaining distance to target: 0.70761997
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 5
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5494053 j: -1.5916157, k: -8.487497
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 11

// in this tick (12) the deceleration should have been enabled
c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 12
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 4, melee action tick index: 6, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.468462, j: 5.866131, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000008, remaining distance to target: 0.5080751
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 4
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5498025 j: -1.5913931, k: -8.487498
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 12

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 13
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 5, melee action tick index: 6, melee max counter: 23
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -10.468462, j: 5.866131, k: 1.7588289e-05, decelerating?: false, magnitude: 12.000008, remaining distance to target: 0.30900168
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 3
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.550611 j: -1.5909401, k: -8.487498
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 13

// not in this tick
c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 14
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 6, melee action tick index: 6, melee max counter: 23
c_character_physics_mode_melee_datum::melee_deceleration_fixup - current velocity: 0.20000014 >= min_velocity: 0.012500001
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -8.014916, j: 4.4912567, k: 1.3466034e-05, decelerating?: true, magnitude: 9.187507, remaining distance to target: 0.11136652
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 0
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5526742 j: -1.589784, k: -8.487499
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 14

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 15
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 7, melee action tick index: 6, melee max counter: 23
c_character_physics_mode_melee_datum::melee_deceleration_fixup - current velocity: 0.15122512 >= min_velocity: 0.012500001
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -5.561371, j: 3.1163826, k: 9.343781e-06, decelerating?: true, magnitude: 6.375005, remaining distance to target: 0.008554129
c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 0
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: -1.5965656 j: -1.565189, k: -8.487499
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 15

// a velocity of 6.375005 (magnitude: 6.375005) is left outside the melee physics mode unhandled
// causing the player to go further than it should

***** 60 tickrate SWORD max target distance log end
*/