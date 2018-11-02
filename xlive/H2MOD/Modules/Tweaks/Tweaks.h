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
	void applyPlayersActionsUpdateRatePatch();
	void setSens(InputType input_type, int sens);
}
