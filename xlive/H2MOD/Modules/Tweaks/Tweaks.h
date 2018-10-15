#pragma once

enum InputType 
{
	CONTROLLER = 1,
	MOUSE = 0
};

void InitH2Tweaks();
void DeinitH2Tweaks();


namespace H2Tweaks {
	void toggleKillVolumes(bool enable);
	void enableAI_MP();
	void disableAI_MP();
	void PatchPingMeterCheck();
	void FixRanksIcons();
	void applyShaderTweaks();
	void enable60FPSCutscenes();
	void disable60FPSCutscenes();
	void setFOV(int field_of_view_degrees);
	void setCrosshairPos(float crosshair_offset);
	void setCrosshairSize(int size, bool preset);
	void RadarPatch();
	void applyPlayersActionsUpdateRatePatch();
	void setSens(InputType input_type, int sens);
}
