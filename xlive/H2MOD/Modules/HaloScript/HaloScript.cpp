#include "HaloScrpit.h"
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

	void Initialize()
	{
		c_unit_kill		  =	h2mod->GetAddress<p_unit_kill*>(0x13B514, 0x12A363);
		c_unit_in_vehicle = h2mod->GetAddress<p_unit_in_vehicle*>(0x1846D9, 0x16E775);
		c_unit_get_health = h2mod->GetAddress<p_unit_get_health*>(0x184477, 0x165E13);
		c_unit_get_shield = h2mod->GetAddress<p_unit_get_shield*>(0x18447C, 0x16E518);
	}
}