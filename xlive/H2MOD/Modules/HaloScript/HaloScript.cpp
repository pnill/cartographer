#include "HaloScript.h"
#include "H2MOD.h"
namespace HaloScript
{
	typedef int(__cdecl p_unit_kill)(datum UnitDatum);
	p_unit_kill* c_unit_kill;
	void UnitKill(datum UnitDatum)
	{
		c_unit_kill(UnitDatum);
	}
	
	typedef bool(__cdecl p_unit_in_vehicle)(datum UnitDatum);
	p_unit_in_vehicle* c_unit_in_vehicle;
	bool UnitInVehicle(datum UnitDatum)
	{
		return c_unit_in_vehicle(UnitDatum);
	}

	typedef float(__cdecl p_unit_get_health)(datum UnitDatum);
	p_unit_get_health* c_unit_get_health;
	float UnitGetHealth(datum UnitDatum)
	{
		return c_unit_get_health(UnitDatum);
	}

	typedef float(__cdecl p_unit_get_shield)(datum UnitDatum);
	p_unit_get_shield* c_unit_get_shield;
	float UnitGetShield(datum UnitDatum)
	{
		return c_unit_get_shield(UnitDatum);
	}

	typedef void(__cdecl p_physics_set_gravity)(float Multiplier);
	p_physics_set_gravity* c_physics_set_gravity;
	void PhysicsSetGravity(float Multiplier)
	{
		c_physics_set_gravity(Multiplier);
	}

	typedef void(__cdecl p_physics_set_velocity_frame)(float unk1, float unk2, float unk3);
	p_physics_set_velocity_frame* c_physics_set_velocity_frame;
	void PhysicsSetVelocityFrame(float unk1, float unk2, float unk3)
	{
		c_physics_set_velocity_frame(unk1, unk2, unk3);
	}

	typedef void(__cdecl p_render_lights_enable_cinematic_shadow)(bool unk1, datum ObjectDatum, string_id StringId, float unk2);
	p_render_lights_enable_cinematic_shadow* c_render_lights_enable_cinematic_shadow;
	void RenderLightsEnableCinenaticShadow(bool unk1, datum ObjectDatum, string_id StringId, float unk2)
	{
		c_render_lights_enable_cinematic_shadow(unk1, ObjectDatum, StringId, unk2);
	}
	typedef void(__cdecl p_object_destroy)(datum object_datum_index);
	p_object_destroy* c_object_destroy;
	void ObjectDestroy(datum object_datum_index)
	{
		c_object_destroy(object_datum_index);
	}

	void Initialize()
	{
		c_unit_kill	= Memory::GetAddress<p_unit_kill*>(0x13B514, 0x12A363);
		c_unit_in_vehicle = Memory::GetAddress<p_unit_in_vehicle*>(0x1846D9, 0x16E775);
		c_unit_get_health = Memory::GetAddress<p_unit_get_health*>(0x184477, 0x165E13);
		c_unit_get_shield = Memory::GetAddress<p_unit_get_shield*>(0x18447C, 0x16E518);
		c_physics_set_gravity	= Memory::GetAddress<p_physics_set_gravity*>(0xB3C00, 0xA3E13);
		c_physics_set_velocity_frame = Memory::GetAddress<p_physics_set_velocity_frame*>(0xB3D5B, 0xA3F6E);
		c_render_lights_enable_cinematic_shadow = Memory::GetAddress<p_render_lights_enable_cinematic_shadow*>(0x19245A);
		c_object_destroy = Memory::GetAddress<p_object_destroy*>(0xFDCFD, 0x124ED5);
	}
}