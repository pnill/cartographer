#pragma once

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
}
