
#include "MeleeLunge.h"

#include "Util\Memory.h"

#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"

#include <float.h>
#pragma fenv_access (on)

#define MELEE_DEBUG 0

#if MELEE_DEBUG
#define LOG_TRACE_MELEE(msg, ...) \
 ((void) ((LOG_TRACE_GAME(msg, __VA_ARGS__), true))) \

#else
#define LOG_TRACE_MELEE(msg, ...)
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
float compute_something(float v1, float v2)
{
	return ((v1 - get_melee_acceleration(v2)) * 3.0f) / 2.0f;
}

__declspec(naked) void melee_force_decelerate_fixup()
{
	// xmm0 - output value
	// when this code is executed, no floating point values are stored on the FPU stack, no need to preserve anything
	__asm
	{
		// push general purpose registers to stack
		PUSHAD
		PUSHFD

		sub esp, 4

		sub esp, 16 * 4 // allocate 4 xmm registers size on stack (each has 128 bits / 16 bytes)
		movaps[esp + 16 * 0], xmm3 // then copy them to stack
		movaps[esp + 16 * 1], xmm2
		movaps[esp + 16 * 2], xmm1
		movaps[esp + 16 * 3], xmm0

		sub esp, 4 // allocate 4 bytes of memory
		call get_ticks_difference_real // __cdecl, returns float on FPU stack
		fstp dword ptr[esp]

		movaps xmm0, [esp + 4 + 48] // restore xmm0
		mulss xmm0, [esp]

		add esp, 4

		movaps xmm3, [esp + 16 * 0]
		movaps xmm2, [esp + 16 * 1]
		movaps xmm1, [esp + 16 * 2]

		// original instructions
		subss   xmm0, xmm2
		mulss   xmm0, xmm1

		// reset stack after restoring xmm register
		add esp, 16 * 4

		add esp, 4

		// re-load registers as well
		POPFD
		POPAD

		retn
	}
}

__declspec(naked) void melee_limit_some_random_vector()
{
	static float limit = 0.5f;

	__asm
	{
		PUSHAD // 256 bytes
		PUSHFD // 4 bytes

		sub esp, 8 // align the stack to 16 bytes for movaps instruction, otherwise exception is triggered

		sub esp, 16 * 8 // preserve all xmm registers (128 bit * 8 registers)
		//movaps[esp + 16 * 0], xmm0 // then copy them to stack
		movaps[esp + 16 * 1], xmm1
		movaps[esp + 16 * 2], xmm2
		movaps[esp + 16 * 3], xmm3
		movaps[esp + 16 * 4], xmm4
		movaps[esp + 16 * 5], xmm5
		movaps[esp + 16 * 6], xmm6
		movaps[esp + 16 * 7], xmm7

		lea eax, dword ptr[esi + 0x1C]
		fld[limit]		// load the limit value
		sub esp, 4		// allocate memory
		fstp[esp]		// load it in esp
		push eax		// push vector pointer
		call limit3d	// self explanatory
		add esp, 8		// reset stack after limit3d call

		//movaps xmm0, [esp + 16 * 0]
		movaps xmm1, [esp + 16 * 1]
		movaps xmm2, [esp + 16 * 2]
		movaps xmm3, [esp + 16 * 3]
		movaps xmm4, [esp + 16 * 4]
		movaps xmm5, [esp + 16 * 5]
		movaps xmm6, [esp + 16 * 6]
		movaps xmm7, [esp + 16 * 7]
		add esp, 16 * 8

		add esp, 8

		POPFD
		POPAD

		movss xmm0, dword ptr[esi + 0x48]

		ret
	}
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

		real_vector3d localized_velocity = {};
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

float __cdecl get_max_melee_lunge_speed_per_tick(float target_distance, char weapon_is_sword)
{
	auto p_get_max_melee_lunge_speed_per_tick = Memory::GetAddressRelative<decltype(&get_max_melee_lunge_speed_per_tick)>(0x50B06F, 0x4FD69F);
	
	float flt_ret = 0.0f;
	__asm
	{
		sub esp, 4
		movss[esp], xmm0
		push eax
		xor eax, eax
		mov al, weapon_is_sword
		push eax
		push target_distance
		call p_get_max_melee_lunge_speed_per_tick // __usercall, returns float in xmm register instead of fpu stack
		add esp, 8
		movss[flt_ret], xmm0 // move xmm0 on stack
		movss xmm0, [esp]
		pop eax
		add esp, 4
	}

	return flt_ret;
}

/*
NOTES:

	** At higher tickrates than 30 (at least double or higher), the lunge update executes twice or more ticks compared with 30 tick
	** And in theory it should move the player a shorter distance per tick over the same distance
*/

/*
	outputs new flags
*/

#define FINISH_DECELERATION() \
	out_current_flags |= FLAG(melee_deceleration_finished); \
	this->m_time_to_target_in_ticks = 0; \
	output->out_translational_velocity = *current_velocity; \

void c_character_physics_mode_melee_datum::melee_deceleration_fixup
(
	character_physics_output* output,
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
	float remaining_distance_from_player_position = target_vector.magnitude();
	float max_speed_per_tick = get_max_melee_lunge_speed_per_tick(m_distance, m_weapon_is_sword);

	float current_velocity_and_aiming_vector_dot_product = m_aiming_direction.dot_product(*current_velocity);
	current_velocity_and_aiming_vector_dot_product *= time_globals::get_seconds_per_tick();

	if (this->m_started_decelerating)
	{
		// preserve default output by original code
		real_vector3d default_output = output->out_translational_velocity;

		// copy current_velocity in output_velocity
		output->out_translational_velocity = *current_velocity;

		// get the velocity per game logic update
		output->out_translational_velocity = output->out_translational_velocity * time_globals::get()->seconds_per_tick;

		real_vector3d direction = *current_velocity * time_globals::get_seconds_per_tick();

		// get the direction of the melee lunge and the magnitude as well
		float normalized_current_magnitude_per_tick = normalize3d(&direction);

		// this variable is named max_speed_per_tick_2 because it should result in the same value after processing in `compute something`
		// because passing both arguments with the same value will cause that
		float max_speed_per_tick_2 = compute_something(max_speed_per_tick, max_speed_per_tick);

		float real_time_to_target = ((remaining_distance_from_player_position / max_speed_per_tick_2) - 0.5f);

		// field_28 is always the same after the first melee tick
		float unk1 = m_field_28 * 1.5;
		double unk2;
		if (unk1 >= 0.75)
		{
			if (unk1 <= 3.5)
				unk2 = unk1;
			else
				unk2 = 3.5;
		}
		else
		{
			unk2 = 0.75;
		}

		float maybe_minimum_velocity = time_globals::get_seconds_per_tick() * unk2;

		float current_velocity_per_tick = output->out_translational_velocity.magnitude();

		float velocity_to_decelerate_per_tick = this->m_velocity_to_decelerate / k_deceleration_ticks_real;

		//float decelerated_velocity = (float)out_deceleration_ticks * velocity_to_decelerate_per_tick;

		// TODO: this does not work perfectly
		// do some hack calculations to compute minimum velocity
		/*float initial_velocity_before_deceleration = this->m_velocity_to_decelerate + unk3;
		float minimum_velocity_per_tick = initial_velocity_before_deceleration - this->m_velocity_to_decelerate;
		if (unk3 > initial_velocity_before_deceleration - unk3)
			minimum_velocity_per_tick = 0.0f;*/

		if (velocity_to_decelerate_per_tick > current_velocity_per_tick)
		{
			velocity_to_decelerate_per_tick = current_velocity_per_tick;
		}

		//const float melee_max_cosine = cos(degreesToRadians(85.f));
		const float melee_max_cosine = 0.087155744f;

		if (get_max_melee_lunge_speed_per_tick(this->m_distance, this->m_weapon_is_sword) <= k_valid_real_epsilon)
		{
			FINISH_DECELERATION();
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

		if (!(out_current_flags & FLAG(melee_deceleration_finished)))
		{
			// compare 2 dot products
			if ((aiming_vector->dot_product(direction) <= (current_velocity_per_tick * melee_max_cosine) 
				|| normalized_current_magnitude_per_tick == 0.0f))
			{
				float unk4 = (this->m_velocity_to_decelerate + maybe_minimum_velocity) / 3.0f;
				float deceleration = current_velocity_per_tick;
				if (unk4 <= current_velocity_per_tick)
					deceleration = unk4;

				deceleration *= time_globals::get_ticks_difference_real();

				// im not entirely sure if this is needed or not
				output->out_translational_velocity = output->out_translational_velocity * time_globals::get_ticks_difference_real();

				output->out_translational_velocity = (direction * (-deceleration)) + output->out_translational_velocity;

				// this shit apparently is not converted back to a vector that
				// - defines the velocity in units per seconds intead of units per tick
				// either some numb nut at bungie forgot to add the line bellow or it was intentional
				// so we just convert the current velocity and deceleration to 30 tick values
				// and output them like that
				//output->out_translational_velocity = output->out_translational_velocity * (float)time_globals::get()->ticks_per_second;

				if (maybe_minimum_velocity <= ((current_velocity_per_tick - unk4) + k_valid_real_epsilon))
				{
					return;
				}

				out_current_flags |= FLAG(melee_deceleration_finished);
				this->m_time_to_target_in_ticks = 0;
				return;
			}
			else
			{
				if (current_velocity_per_tick <= k_valid_real_epsilon)
				{
					FINISH_DECELERATION();
					return;
				}
			
				this->m_time_to_target_in_ticks = (int)real_time_to_target;

				//this->m_time_to_target_in_ticks = (current_velocity_per_tick / velocity_to_decelerate_per_tick) - 0.5f;
				output->out_translational_velocity = (direction * (-velocity_to_decelerate_per_tick)) + output->out_translational_velocity;

				//this->m_time_to_target_in_ticks = ((_FP5 * 0.5f) + v124);
				//addDebugText("remaining target distance ticks: %f, %d", ((_FP5 * 0.5f) + v124), this->m_time_to_target_in_ticks);

				//out_deceleration_ticks++;


				//addDebugText("remaining target distance ticks h2 code: %f, %d", ((remaining_distance / something1) - 0.5f), this->m_time_to_target_in_ticks);

				output->out_translational_velocity = output->out_translational_velocity * (float)time_globals::get()->ticks_per_second;
				return;
			}
		}

		output->out_translational_velocity = *current_velocity;
		this->m_time_to_target_in_ticks = 0;
		return;
	}
}

#undef MAX_DECELERATION

bool c_character_physics_mode_melee_datum::pin_localized_velocity(real_vector3d* output, real_vector3d* localized_velocity)
{
	float output_velocity_magnitude = output->magnitude();
	float localized_velocity_magnitude = localized_velocity->magnitude() + 0.80000001f;

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
		float dot_product = output->dot_product(*localized_velocity);
		float unk2 = fminf(magnitude - 1.2f, -dot_product);
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
	character_physics_output* a2,
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

	auto p_character_physics_mode_melee_datum_update_internal = Memory::GetAddressRelative<void(__thiscall*)(c_character_physics_mode_melee_datum*, character_physics_output*, char, float, real_vector3d*, real_point3d*, real_vector3d*, real_vector3d*, real_point3d*)>(0x50B0D1, 0x4FD701);

	if (time_globals::get()->ticks_per_second <= 30)
		return p_character_physics_mode_melee_datum_update_internal(this, a2, a3, distance_world_units, target_translational_velocity, target_origin, aiming_vector, translational_velocity, object_origin);

	// for now prevent this code from executing if we are using a sword
	// the issue with it is quite dificult to reverse and fix
	//if (this->m_weapon_is_sword)
		//return p_character_physics_mode_melee_datum_update_internal(this, a2, a3, distance_world_units, target_translational_velocity, target_origin, aiming_vector, translational_velocity, object_origin);

	// if deceleration has started and max accel tick count - melee tick is bellow 0, it means 4 deceleration ticks have been executed

	real_vector3d* out_velocity = &a2->out_translational_velocity;

	// we keep some flags in m_started_decelerationg (first 4 bits)
	BYTE melee_flags = this->m_started_decelerating & melee_flags_mask;

	this->m_started_decelerating &= ~melee_flags_mask;

	BYTE melee_deceleration_tick_count = 0;

	LOG_TRACE_MELEE("{} : start melee log @ tick {}", __FUNCTION__, m_melee_tick);

	// this is set to localized_velocity, from biped_dash_hook
	//real_vector3d localized_velocity = field_1C;

	// update melee lunge tick
	p_character_physics_mode_melee_datum_update_internal(this, a2, a3, distance_world_units, target_translational_velocity, target_origin, aiming_vector, translational_velocity, object_origin);

	if (m_melee_tick == 1)
	{
		int added_ticks = m_maximum_counter - 6 - (m_weapon_is_sword ? 7 : 1);
		LOG_TRACE_MELEE("{} - added tick count to maximum counter: {}", __FUNCTION__, added_ticks);

		LOG_TRACE_MELEE("{} - target_distance: {}, maximum counter: {}", __FUNCTION__, distance_world_units, m_maximum_counter);
	}

	if ((float)(m_maximum_counter - m_melee_tick) <= k_deceleration_ticks_real && !m_started_decelerating)
	{
		LOG_TRACE_MELEE("{} : we are about to start decelerating @ next tick : {}", __FUNCTION__, m_melee_tick);
	}

	if (a3)
		melee_deceleration_fixup(a2, object_origin, translational_velocity, aiming_vector, melee_flags, melee_deceleration_tick_count);

	if (distance_world_units > 5.0f
		|| melee_flags & FLAG(melee_deceleration_finished)
		|| m_time_to_target_in_ticks <= 0
		|| m_melee_tick >= (m_maximum_counter + 6)
		)
	{
		//pin_localized_velocity(&a2->out_translational_velocity, &localized_velocity);
		a2->flags |= FLAG(2); // set to let the engine know we should get out of the melee lunge physics
	}
	else
	{
		a2->flags &= ~FLAG(2); // maintain lunge physics
	}

	// create vector from subtracting 1 point out of the other 
	real_vector3d distance_vector = m_target_point - *object_origin;
	float remaining_distance = distance_vector.magnitude();

	float unk_float_distance = this->m_aiming_direction.dot_product(*translational_velocity);
	unk_float_distance *= time_globals::get_seconds_per_tick();
	float unk_velocity = compute_something(unk_float_distance, get_max_melee_lunge_speed_per_tick(this->m_distance, this->m_weapon_is_sword));
	if (unk_velocity < 0.0f)
		unk_velocity = 0.0f;

	float log_magnitude = a2->out_translational_velocity.magnitude();

	LOG_TRACE_MELEE("{} : output velocity:       i: {}, j: {}, k: {}, magnitude: {}, decelerating?: {}, remaining distance to target: {} \n",
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
		m_aiming_direction.magnitude());

	LOG_TRACE_MELEE("{} : previous velocity: {}, previous velocity dot product with aiming vector adjusted: {}",
		__FUNCTION__,
		translational_velocity->magnitude(),
		m_aiming_direction.dot_product(*translational_velocity));

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

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 16777224
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 2, melee action tick index: 3, melee max counter: 16
c_character_physics_mode_melee_datum::melee_deceleration_fixup - current velocity: 0.30625013 >= min_velocity: 0.025000002
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -1.8039411, j: 6.1144466, k: -2.5331977e-07, decelerating?: true, magnitude: 6.375003, remaining distance to target: 0.06264523

c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 0
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.23336934 j: -0.31038895, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 8

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 33554441
c_character_physics_mode_melee_datum::melee_deceleration_fixup - melee animation tick: 3, melee action tick index: 3, melee max counter: 16
c_character_physics_mode_melee_datum::melee_deceleration_fixup - current velocity: 0.1061288 >= min_velocity: 0.025000002
c_character_physics_mode_melee_datum::update_internal : output velocity:       i: -0.10508454, j: 0.35618508, k: 3.5188577e-07, decelerating?: true, magnitude: 0.37136313, remaining distance to target: 0.0007751883

c_character_physics_mode_melee_datum::update_internal : remaining distance in ticks: 0
c_character_physics_mode_melee_datum::update_internal : vector point1_unk: i: 0.26417547 j: -0.41480565, k: -8.485
c_character_physics_mode_melee_datum::update_internal : ended log for @ melee tick 9

c_character_physics_mode_melee_datum::update_internal : about to log information @ melee tick 50331658
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

***** 60 tickrate SWORD max target distance log end
*/