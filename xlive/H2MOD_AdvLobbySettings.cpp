#include "H2MOD_AdvLobbySettings.h"
#include <h2mod.pb.h>
#include "Globals.h"
#include "H2OnscreenDebugLog.h"
#include "GSCustomMenu.h"
#include "H2Tweaks.h"

bool AdvLobbySettings_mp_explosion_physics = false;
bool AdvLobbySettings_mp_sputnik = false;
bool AdvLobbySettings_mp_grunt_bday_party = false;
bool AdvLobbySettings_grenade_chain_react = false;
bool AdvLobbySettings_banshee_bomb = false;
char AdvLobbySettings_mp_blind = false;
bool AdvLobbySettings_flashlight = false;
bool AdvLobbySettings_disable_kill_volumes = false;

static void refresh_mp_explosion_physics() {
	if (!h2mod->Server) {
		if (AdvLobbySettings_mp_explosion_physics || AdvLobbySettings_mp_sputnik)
			WriteValue(h2mod->GetBase() + 0x0017a44b, (BYTE)0x1e);
		else
			WriteValue(h2mod->GetBase() + 0x0017a44b, (BYTE)0);
	}
}

static bool justGottaSendIt = false;
static bool justDontSendIt = false;
static std::chrono::time_point<std::chrono::system_clock> lastReq;
static std::chrono::time_point<std::chrono::system_clock> firstReq;

static void actuallySendPacket() {
	if (!gameManager->isHost() || h2mod->get_engine_type() != EngineType::MULTIPLAYER_ENGINE)
		return;

	TRACE_GAME("[h2mod] Sending AdvLobbySettings.");
	addDebugText("[h2mod] Sending AdvLobbySettings.");

#ifdef _DEBUG
	int tmpFlagOrig = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	int tmpFlag = tmpFlagOrig;
	tmpFlag &= 0xFFFFFFFF ^ (_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetDbgFlag(tmpFlag);
#endif

	H2ModPacket teampak;
	teampak.set_type(H2ModPacket_Type_map_info_request);

	h2mod_lobby_settings* lobby_settings = teampak.mutable_lobby_settings();

	lobby_settings->set_mp_explosion_physics((int)AdvLobbySettings_mp_explosion_physics + 1);
	lobby_settings->set_mp_sputnik((int)AdvLobbySettings_mp_sputnik + 1);
	lobby_settings->set_mp_grunt_bday_party((int)AdvLobbySettings_mp_grunt_bday_party + 1);
	lobby_settings->set_grenade_chain_react((int)AdvLobbySettings_grenade_chain_react + 1);
	lobby_settings->set_banshee_bomb((int)AdvLobbySettings_banshee_bomb + 1);
	lobby_settings->set_mp_blind((int)AdvLobbySettings_mp_blind + 1);
	lobby_settings->set_flashlight((int)AdvLobbySettings_flashlight + 1);

	network->send_h2mod_packet(teampak);

#ifdef _DEBUG
	_CrtSetDbgFlag(tmpFlagOrig);
#endif

	refresh_mp_explosion_physics();
}

void AdvLobbySettings::sendLobbySettingsPacket()
{
	if (!gameManager->isHost() || h2mod->get_engine_type() != EngineType::MULTIPLAYER_ENGINE)
		return;
	
	lastReq = std::chrono::system_clock::now() + std::chrono::milliseconds(4000);
	if (!justGottaSendIt)
		firstReq = std::chrono::system_clock::now();
	justGottaSendIt = true;
	justDontSendIt = false;
}

void AdvLobbySettings::resetLobbySettings()
{
	if (h2mod->Server)
		return;

	TRACE_GAME("[h2mod] Resetting AdvLobbySettings.");
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

void AdvLobbySettings::parseLobbySettings(const h2mod_lobby_settings& lobby_settings)
{
	if (h2mod->Server)
		return;

	TRACE_GAME("[h2mod] Parsing AdvLobbySettings.");
	addDebugText("[h2mod] Parsing AdvLobbySettings.");
	
	AdvLobbySettings_mp_explosion_physics = lobby_settings.mp_explosion_physics() - 1;
	AdvLobbySettings_mp_sputnik = lobby_settings.mp_sputnik() - 1;
	AdvLobbySettings_mp_grunt_bday_party = lobby_settings.mp_grunt_bday_party() - 1;
	AdvLobbySettings_grenade_chain_react = lobby_settings.grenade_chain_react() - 1;
	AdvLobbySettings_banshee_bomb = lobby_settings.banshee_bomb() - 1;
	AdvLobbySettings_mp_blind = lobby_settings.mp_blind() - 1;
	AdvLobbySettings_flashlight = lobby_settings.flashlight() - 1;

	refresh_mp_explosion_physics();
}

void AdvLobbySettings::loop() {
	if (justGottaSendIt) {
		if (std::chrono::duration_cast<std::chrono::milliseconds>(lastReq - std::chrono::system_clock::now()).count() < 0) {
			firstReq = std::chrono::system_clock::now();
			justGottaSendIt = false;
			if (!justDontSendIt)
				actuallySendPacket();
			justDontSendIt = false;
		}
		else if (std::chrono::duration_cast<std::chrono::milliseconds>(firstReq - std::chrono::system_clock::now()).count() < -4000) {
			lastReq = std::chrono::system_clock::now() + std::chrono::milliseconds(4000);
			firstReq = std::chrono::system_clock::now();
			justGottaSendIt = true;
			justDontSendIt = true;
			actuallySendPacket();
		}
	}
}
