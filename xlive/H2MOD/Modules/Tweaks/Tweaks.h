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
	void applyShaderTweaks();
	void enable60FPSCutscenes();
	void disable60FPSCutscenes();
	void setFOV(double field_of_view_degrees);
	void setVehicleFOV(double field_of_view_degrees);
	void setHz();
	void setCrosshairPos(float crosshair_offset);
	void setCrosshairSize(int size, bool preset);
	void applyObjectPredictionPatch();
	void setSens(InputType input_type, int sens);
	void setSavedSens();
	void sunFlareFix();
}
