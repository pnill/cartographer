#pragma once

void InitH2Tweaks();
void DeinitH2Tweaks();

namespace H2Tweaks {
	void toggleKillVolumes(bool enable);
	void toggleAiMp(bool toggle);
	void toggleUncappedCampaignCinematics(bool toggle);
	void setHz();
	void applyMeleePatch(bool toggle);
	void setSens(std::string input_type, int sens);
	void setSavedSens();
	void sunFlareFix();
	void WarpFix(bool enable);
}
