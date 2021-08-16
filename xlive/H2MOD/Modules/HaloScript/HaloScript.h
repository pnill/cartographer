#pragma once
#include "Blam/Cache/DataTypes/StringID.h"

namespace HaloScript
{
	void UnitKill(datum UnitDatum);
	bool UnitInVehicle(datum UnitDatum);
	float UnitGetHealth(datum UnitDatum);
	float UnitGetShield(datum UnitDatum);

	void PhysicsSetGravity(float Multiplier);
	void PhysicsSetVelocityFrame(float unk1, float unk2, float unk3);
	void RenderLightsEnableCinenaticShadow(bool unk1, datum ObjectDatum, string_id StringId, float unk2);
	void ObjectDestroy(datum object_datum_index);
	void Initialize();
}
