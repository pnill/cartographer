#pragma once


namespace HudElements
{
	void ApplyHooks();
	void RadarPatch();
	void ToggleFirstPerson(bool state = NULL);
	void ToggleHUD(bool state = NULL);
	void setCrosshairSize();
	void setCrosshairPos();
	void setFOV();
	void setVehicleFOV();
	void OnMapLoad();
	void Init();
}