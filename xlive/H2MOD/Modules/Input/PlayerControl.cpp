#include "PlayerControl.h"
#include "H2MOD/Modules/Config/Config.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/EventHandler/EventHandler.h"
#include "Blam/Engine/Players/PlayerControls.h"

typedef void __cdecl c_UpdatePlayerControl(float yaw, float pitch);
c_UpdatePlayerControl* p_UpdatePlayerControl;

void __cdecl UpdatePlayerControl(float yawChange, float pitchChange)
{
	p_UpdatePlayerControl(yawChange, pitchChange);
	EventHandler::executePlayerControlCallback(yawChange, pitchChange);
}

void PlayerControl::ApplyHooks()
{
	p_UpdatePlayerControl = h2mod->GetAddress<c_UpdatePlayerControl*>(0x90D62);
	PatchCall(h2mod->GetAddress(0x93596), UpdatePlayerControl);
	PatchCall(h2mod->GetAddress(0x9390D), UpdatePlayerControl);
}

Blam::EngineDefinitions::Players::s_gamestate_player_controls* PlayerControl::GetControls(int local_player_index)
{
	return *h2mod->GetAddress<Blam::EngineDefinitions::Players::s_gamestate_player_controls**>(0x4ca37c + (local_player_index * sizeof(Blam::EngineDefinitions::Players::s_gamestate_player_controls)));
}
