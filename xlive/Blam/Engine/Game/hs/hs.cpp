#include "stdafx.h"
#include "hs.h"

namespace HaloScript
{
	typedef int(__cdecl unit_kill_t)(datum unitDatum);
	unit_kill_t* p_unit_kill;
	void UnitKill(datum unitDatum)
	{
		p_unit_kill(unitDatum);
	}
	
	typedef bool(__cdecl unit_in_vehicle_t)(datum unitDatum);
	unit_in_vehicle_t* p_unit_in_vehicle;
	bool UnitInVehicle(datum unitDatum)
	{
		return p_unit_in_vehicle(unitDatum);
	}

	typedef float(__cdecl unit_get_health_t)(datum unitDatum);
	unit_get_health_t* p_unit_get_health;
	float UnitGetHealth(datum unitDatum)
	{
		return p_unit_get_health(unitDatum);
	}

	typedef float(__cdecl unit_get_shield_t)(datum unitDatum);
	unit_get_shield_t* p_unit_get_shield;
	float UnitGetShield(datum unitDatum)
	{
		return p_unit_get_shield(unitDatum);
	}

	typedef void(__cdecl physics_set_gravity_t)(float multiplier);
	physics_set_gravity_t* p_physics_set_gravity;
	void PhysicsSetGravity(float Multiplier)
	{
		p_physics_set_gravity(Multiplier);
	}

	typedef void(__cdecl physics_set_velocity_frame_t)(float unk1, float unk2, float unk3);
	physics_set_velocity_frame_t* p_physics_set_velocity_frame;
	void PhysicsSetVelocityFrame(float unk1, float unk2, float unk3)
	{
		p_physics_set_velocity_frame(unk1, unk2, unk3);
	}

	typedef void(__cdecl render_lights_enable_cinematic_shadow_t)(bool unk1, datum objectDatum, string_id StringId, float unk2);
	render_lights_enable_cinematic_shadow_t* p_render_lights_enable_cinematic_shadow;
	void RenderLightsEnableCinenaticShadow(bool unk1, datum objectDatum, string_id StringId, float unk2)
	{
		p_render_lights_enable_cinematic_shadow(unk1, objectDatum, StringId, unk2);
	}

	typedef void(__cdecl object_destroy_t)(datum object_datum_index);
	object_destroy_t* p_object_destroy;
	void ObjectDestroy(datum object_datum_index)
	{
		p_object_destroy(object_datum_index);
	}

	void Initialize()
	{
		p_unit_kill	= Memory::GetAddress<unit_kill_t*>(0x13B514, 0x12A363);
		p_unit_in_vehicle = Memory::GetAddress<unit_in_vehicle_t*>(0x1846D9, 0x16E775);
		p_unit_get_health = Memory::GetAddress<unit_get_health_t*>(0x184477, 0x165E13);
		p_unit_get_shield = Memory::GetAddress<unit_get_shield_t*>(0x18447C, 0x16E518);
		p_physics_set_gravity	= Memory::GetAddress<physics_set_gravity_t*>(0xB3C00, 0xA3E13);
		p_physics_set_velocity_frame = Memory::GetAddress<physics_set_velocity_frame_t*>(0xB3D5B, 0xA3F6E);
		p_render_lights_enable_cinematic_shadow = Memory::GetAddress<render_lights_enable_cinematic_shadow_t*>(0x19245A);
		p_object_destroy = Memory::GetAddress<object_destroy_t*>(0xFDCFD, 0x124ED5);
	}
}