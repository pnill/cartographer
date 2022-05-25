#pragma once
#include "Blam/Cache/DataTypes/StringID.h"

namespace HaloScript
{
	void UnitKill(datum unitDatum);
	bool UnitInVehicle(datum unitDatum);
	float UnitGetHealth(datum unitDatum);
	float UnitGetShield(datum unitDatum);

	void PhysicsSetGravity(float Multiplier);
	void PhysicsSetVelocityFrame(float unk1, float unk2, float unk3);
	void RenderLightsEnableCinenaticShadow(bool unk1, datum objectDatum, string_id StringId, float unk2);
	void ObjectDestroy(datum object_datum_index);
	void Initialize();
}
