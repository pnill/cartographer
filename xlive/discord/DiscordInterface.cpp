#include <unordered_set>
#include "stdafx.h"
#include "discord_rpc.h"
#include "discord_register.h"
#include "DiscordInterface.h"

const static char api_key[] = "379371722685808641";

char state[128] = "";
char details[128] = "Discord Api in invalid state! How did you do that?";
char map_key[32] = "default";
char map_mouse_over[128] = "";
char mode_key[32] = "";
char mode_details[128] = "";
bool inited = false;
int current_player_count = 0;
int max_player_count = 0;
bool hide_players = false;

// AFAIK there is no easy way to get the list of uploaded assets using the API
const static std::unordered_set<std::string> maps_with_images = {
	// logo
	"default",
	// singleplayer maps
	"00a_introduction",
	"01a_tutorial",
	"01b_spacestation",
	"03a_oldmombasa",
	"03b_newmombasa",
	"04a_gasgiant",
	"04b_floodlab",
	"05a_deltaapproach",
	"05b_deltatowers",
	"06a_sentinelwalls",
	"06b_floodzone",
	"07a_highcharity",
	"07b_forerunnership",
	"08a_deltacliffs",
	"08b_deltacontrol",
	// multiplayer maps
	"ascension",
	"backwash",
	"beavercreek",
	"burial_mounds",
	"coagulation",
	"colossus",
	"containment",
	"cyclotron",
	"deltatap",
	"dune",
	"elongation",
	"foundation",
	"gemini",
	"headlong",
	"lockout",
	"midship",
	"needle",
	"street_sweeper",
	"triplicate",
	"turf",
	"warlock",
	"waterworks",
	"zanzibar"
};

static void updateDiscordPresence()
{
	if (!inited)
		return;

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = state;
	discordPresence.details = details;
	discordPresence.largeImageKey = map_key;
	discordPresence.largeImageText = map_mouse_over;
	discordPresence.smallImageKey = mode_key;
	discordPresence.smallImageText = mode_details;
	if (!hide_players) {
		discordPresence.partySize = current_player_count;
		discordPresence.partyMax = max_player_count;
	}
	else {
		discordPresence.partySize = 0;
		discordPresence.partyMax = 0;
	}

	Discord_UpdatePresence(&discordPresence);
}

static void handleDiscordReady()
{
}

static void handleDiscordDisconnected(int errcode, const char* message)
{
}

static void handleDiscordError(int errcode, const char* message)
{
}

static void handleDiscordJoin(const char* secret)
{
}

static void handleDiscordSpectate(const char* secret)
{
}

static void handleDiscordJoinRequest(const DiscordJoinRequest* request)
{
}

void DiscordInterface::Init()
{
	inited = true;
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = handleDiscordReady;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.errored = handleDiscordError;
	handlers.joinGame = handleDiscordJoin;
	handlers.spectateGame = handleDiscordSpectate;
	handlers.joinRequest = handleDiscordJoinRequest;
	Discord_Initialize(api_key, &handlers, 1, NULL);
	updateDiscordPresence();
}

void DiscordInterface::SetPlayerCountInfo(int current, int max)
{
	if (current_player_count == current && max_player_count == max)
		return;
	current_player_count = current;
	max_player_count = max;
	updateDiscordPresence();
}

void DiscordInterface::SetDetails(const std::string &map_name)
{
	strncpy_s(details, sizeof(details), map_name.c_str(), map_name.size());
	updateDiscordPresence();
}

void DiscordInterface::HidePlayerCount(bool hide)
{
	hide_players = hide;
}

void DiscordInterface::SetGameState(std::string map_id,
	const std::string &game_state,
	const std::string &map_name,
	const std::string &gamemode_id,
	const std::string &gamemode_name,
	const bool hide_player_count)
{
	HidePlayerCount(hide_player_count);
	std::string details = ((map_name.empty() && gamemode_name.empty()) ? "" :
		gamemode_name + " on " + map_name);

	if (!maps_with_images.count(map_id))
		map_id = "unknown_map";

	strncpy_s(map_mouse_over, sizeof(map_mouse_over), map_name.c_str(), map_name.size());
	strncpy_s(map_key, sizeof(map_key), map_id.c_str(), map_id.size());
	strncpy_s(state, sizeof(state), game_state.c_str(), game_state.size());
	strncpy_s(mode_key, sizeof(mode_key), gamemode_id.c_str(), gamemode_id.size());
	strncpy_s(mode_details, sizeof(mode_details), gamemode_name.c_str(), gamemode_name.size());
	SetDetails(details);
}

void DiscordInterface::SetGameMode(const std::string &gamemode_id)
{
	strncpy_s(mode_key, sizeof(mode_key), gamemode_id.c_str(), gamemode_id.size());
	updateDiscordPresence();
}