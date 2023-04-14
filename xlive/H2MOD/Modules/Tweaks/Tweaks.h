#pragma once

// TODO figure out where to move this
struct ui_text_bounds
{
	short top;
	short left;
	short bottom;
	short right;
};

namespace H2Tweaks {
	void ApplyPatches();
	void DisposePatches();
	void SetScreenRefreshRate();
	void SunflareFix();
	void WarpFix(bool enable);
	void RefreshTogglexDelay();
}
