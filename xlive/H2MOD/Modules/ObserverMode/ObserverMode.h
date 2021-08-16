#pragma once

namespace ObserverMode
{
	enum e_observer_mode : byte
	{
		observer_none,
		observer_freecam,
		observer_followcam,
		observer_firstperson
	};
	void NextPlayer();
	void SwitchObserverMode(e_observer_mode mode);
	
	void FollowObject(int local_player_index, int unk);
	void ApplyHooks();
	void Initialize();
}
