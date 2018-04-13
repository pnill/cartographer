#pragma once

#define CONTROLLER 1
#define MOUSE 0

void InitH2Tweaks();
void DeinitH2Tweaks();
void setSens(short input_type, int sens);

class H2Tweaks {
public:
	static void enableAI_MP();
	static void disableAI_MP();
	static void PatchPingMeterCheck();
	static void FixRanksIcons();
	static void applyHitfix();
	static void applyShaderTweaks();
	static void enable60FPSCutscenes();
	static void disable60FPSCutscenes();
	static void setFOV(int field_of_view_degrees);
	static void setCrosshairPos(float crosshair_offset);

private:

};
