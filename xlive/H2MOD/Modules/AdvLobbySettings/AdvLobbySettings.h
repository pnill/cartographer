#pragma once

struct skull_enabled_flags
{
	bool Envy;
	bool Grunt_Birthday_Party;
	bool Assassians;
	bool Thunderstorm;
	bool Famine;
	bool IWHBYD;
	bool Blind;
	bool Ghost;
	bool Black_Eye;
	bool Catch;
	bool Sputnik;
	bool Iron;
	bool Mythic;
	bool Anger;
	bool Whuppopotamus;
};

enum e_skulls
{
	_envy,
	_grunt_birthday_party,
	_assassians,
	_thunderstorm,
	_famine,
	_IWHBYD,
	_blind,
	_ghost,
	_black_eye,
	_catch,
	_sputnik,
	_iron,
	_mythic,
	_anger,
	_whuppopotamus
};

class AdvLobbySettings
{
public:

	void sendLobbySettingsPacket();
	void resetLobbySettings();
	void parseLobbySettings(void* lobby_settings);
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
