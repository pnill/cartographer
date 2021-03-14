#pragma once

void InitH2Tweaks();
void DeinitH2Tweaks();

namespace H2Tweaks {
	void toggleKillVolumes(bool enable);
	void toggleAiMp(bool toggle);
	void toggleUncappedCampaignCinematics(bool toggle);
	void setHz();
	void sunFlareFix();
	void WarpFix(bool enable);
}
