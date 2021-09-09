
#include "H2MOD\Modules\AdvLobbySettings\AdvLobbySettings.h"

#include "H2MOD.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD/Modules/Networking/Networking.h"

#include "Util/Hooks/Hook.h"

bool AdvLobbySettings_mp_explosion_physics = false;
bool AdvLobbySettings_mp_sputnik = false;
bool AdvLobbySettings_mp_grunt_bday_party = false;
bool AdvLobbySettings_grenade_chain_react = false;
bool AdvLobbySettings_banshee_bomb = false;
char AdvLobbySettings_mp_blind = false;
bool AdvLobbySettings_flashlight = false;
bool AdvLobbySettings_disable_kill_volumes = false;

static void refresh_mp_explosion_physics() {
	
	if (!Memory::isDedicatedServer()) {
		if (AdvLobbySettings_mp_explosion_physics || AdvLobbySettings_mp_sputnik)
			WriteValue(Memory::GetAddress(0x17a44b), (BYTE)0x1e);
		else
			WriteValue(Memory::GetAddress(0x17a44b), (BYTE)0);
	}
}

static void actuallySendPacket() {
	if (!NetworkSession::localPeerIsSessionHost() || h2mod->GetEngineType() != e_engine_type::Multiplayer)
		return;

	LOG_TRACE_GAME("[h2mod] Sending AdvLobbySettings.");
	addDebugText("[h2mod] Sending AdvLobbySettings.");

#ifdef _DEBUG
	int tmpFlagOrig = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	int tmpFlag = tmpFlagOrig;
	tmpFlag &= 0xFFFFFFFF ^ (_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetDbgFlag(tmpFlag);
#endif


#ifdef _DEBUG
	_CrtSetDbgFlag(tmpFlagOrig);
#endif

	refresh_mp_explosion_physics();
}

void AdvLobbySettings::resetLobbySettings()
{
	if (Memory::isDedicatedServer())
		return;

	LOG_TRACE_GAME("[h2mod] Resetting AdvLobbySettings.");
	addDebugText("[h2mod] Resetting AdvLobbySettings.");

	AdvLobbySettings_mp_explosion_physics = false;
	AdvLobbySettings_mp_sputnik = false;
	AdvLobbySettings_mp_grunt_bday_party = false;
	AdvLobbySettings_grenade_chain_react = false;
	AdvLobbySettings_banshee_bomb = false;
	AdvLobbySettings_mp_blind = false;
	AdvLobbySettings_flashlight = false;
	AdvLobbySettings_disable_kill_volumes = false;

	refresh_mp_explosion_physics();
}

void AdvLobbySettings::parseLobbySettings(void* lobby_settings)
{
	if (Memory::isDedicatedServer())
		return;

	LOG_TRACE_GAME("[h2mod] Parsing AdvLobbySettings.");
	addDebugText("[h2mod] Parsing AdvLobbySettings.");

	refresh_mp_explosion_physics();
}
