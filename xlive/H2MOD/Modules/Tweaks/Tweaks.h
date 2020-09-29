#pragma once

void InitH2Tweaks();
void DeinitH2Tweaks();

namespace H2Tweaks {
	void toggleKillVolumes(bool enable);
	void toggleAiMp(bool toggle);
	void toggleUncappedCampaignCinematics(bool toggle);
	void setFOV(int field_of_view_degrees);
	void setVehicleFOV(int field_of_view_degrees);
	void setHz();
	void setCrosshairPos(float crosshair_offset);
	void setCrosshairSize(int size, bool preset);
	void setCrosshairSize2();
	void applyMeleePatch(bool toggle);
	void setSens(std::string input_type, int sens);
	void setSavedSens();
	void sunFlareFix();
	void WarpFix(bool enable);
	void setVisualTweaks();
}
