#pragma once
#include <H2MOD\protobuf\h2mod.pb.h>

class AdvLobbySettings
{
public:
	void sendLobbySettingsPacket();
	void resetLobbySettings();
	void parseLobbySettings(const h2mod_lobby_settings& lobby_settings);
	void loop();
private:
	
};

extern bool AdvLobbySettings_mp_explosion_physics;
extern bool AdvLobbySettings_mp_sputnik;
extern bool AdvLobbySettings_mp_grunt_bday_party;
extern bool AdvLobbySettings_grenade_chain_react;
extern bool AdvLobbySettings_banshee_bomb;
extern char AdvLobbySettings_mp_blind;
extern bool AdvLobbySettings_flashlight;
extern bool AdvLobbySettings_disable_kill_volumes;
