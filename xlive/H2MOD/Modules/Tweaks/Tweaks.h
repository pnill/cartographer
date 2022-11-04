#pragma once

void InitH2Tweaks();
void DeinitH2Tweaks();
struct ui_text_bounds
{
	short top;
	short left;
	short bottom;
	short right;
};
namespace H2Tweaks {
	void toggleKillVolumes(bool enable);
	void toggleAiMp(bool toggle);
	void toggleUncappedCampaignCinematics(bool toggle);
	void setHz();
	void SunflareFix();
	void WarpFix(bool enable);
	void RefreshTogglexDelay();
}
