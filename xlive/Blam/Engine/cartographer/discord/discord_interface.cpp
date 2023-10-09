#include "stdafx.h"
#include "discord_interface.h"

#include <discord_game_sdk.h>

#include "Blam/Engine/cseries/cseries_strings.h"
#include "Blam/Engine/game/players.h"
#include "Blam/Engine/interface/user_interface_guide.h"
#include "Blam/Engine/interface/user_interface_networking.h"
#include "Blam/Engine/Networking/couch/xsession_manager.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"

#define DISCORD_CLIENT_ID 1158452985882493049

struct s_discord_data
{
	IDiscordCore* core;
	IDiscordUserManager* users;
	IDiscordActivityManager* activities;
	IDiscordLobbyManager* lobbies;
	DiscordUserId user_id;
};

HANDLE discord_thread = INVALID_HANDLE_VALUE;
LONG g_should_stop = FALSE;
DiscordActivity g_discord_activity = {};
bool g_update_rich_presence = true;


void DISCORD_CALLBACK on_rich_presence_updated(void* data, EDiscordResult res)
{
	assert(res == DiscordResult_Ok);
	return;
}

void discord_interface_zero_player_count(void)
{
	g_discord_activity.party.size.current_size = 0;
	g_discord_activity.party.size.max_size = 0;
	return;
}

void discord_interface_update_details()
{
	c_static_string<512> details;
	details.set(g_discord_activity.assets.small_text);
	details.append(" on ");
	details.append(g_discord_activity.assets.large_text);

	strcpy(g_discord_activity.details, details.get_string());
	g_update_rich_presence = true;
	return;
}

void discord_interface_set_details(const char* details)
{
	strcpy(g_discord_activity.details, details);
	g_update_rich_presence = true;
	return;
}

void discord_interface_set_state(const char* state)
{
	discord_interface_set_player_counts();
	strcpy(g_discord_activity.state, state);
	g_update_rich_presence = true;
	return;
}

void discord_interface_set_small_image(const char* small_image, const char* small_text)
{
	strcpy(g_discord_activity.assets.small_image, small_image);
	strcpy(g_discord_activity.assets.small_text, small_text);
	g_update_rich_presence = true;
	return;
}

void discord_interface_set_large_image(const char* large_image, const char* large_text)
{
	strcpy(g_discord_activity.assets.large_image, large_image);
	strcpy(g_discord_activity.assets.large_text, large_text);
	g_update_rich_presence = true;
	return;
}

void discord_interface_set_map_name(const utf8* scenario_name, const utf8* map_name)
{
	discord_interface_set_large_image(scenario_name, map_name);
	discord_interface_update_details();
	return;
}

void discord_interface_set_variant(e_context_variant variant, const utf8* variant_name)
{
	// Convert difficulty to string
	char number_string[2];
	snprintf(number_string, sizeof(number_string), "%lu", variant);

	// Create image name we select for the difficulty
	c_static_string<16> variant_image_name;
	variant_image_name.set("variant_");
	variant_image_name.append(number_string);

	// Set image name and text
	discord_interface_set_small_image(variant_image_name.get_string(), variant_name);
	discord_interface_update_details();
	return;
}


void discord_interface_set_difficulty(int16 difficulty)
{
	// Make sure we don't go out of bounds
	if (difficulty > 3 || difficulty < 0)
	{
		difficulty = 0;
	}

	// Convert difficulty to string
	char number_string[2];
	snprintf(number_string, sizeof(number_string), "%hd", difficulty);

	// Create image name we select for the difficulty
	c_static_string<16> difficulty_image_name;
	difficulty_image_name.set("diff_");
	difficulty_image_name.append(number_string);

	// Set image name and text
	const char* difficulty_names[4] = { "Easy", "Normal", "Heroic", "Legendary" };
	strcpy(g_discord_activity.assets.small_image, difficulty_image_name.get_string());
	strcpy(g_discord_activity.assets.small_text, difficulty_names[difficulty]);
	discord_interface_update_details();
	g_update_rich_presence = true;
	return;
}

void discord_interface_set_player_counts(void)
{
	s_network_session* session = nullptr;
	if (NetworkSession::GetActiveNetworkSession(&session))
	{
		g_discord_activity.party.size.current_size = session->membership[0].player_count;
		g_discord_activity.party.size.max_size = session->parameters[0].max_party_players;
	}
	else
	{
		discord_interface_zero_player_count();
	}
	g_update_rich_presence = true;
	return;
}

void discord_interface_encode_xsession_info(XSESSION_INFO* session_info)
{
	snprintf(g_discord_activity.secrets.join, sizeof(g_discord_activity.secrets.join), 
		"%llu/%llu/%llu/%lu/%lu/%hu/%lu/%hu/%llu/%llu/%lu", 
		session_info->sessionID,
		session_info->keyExchangeKey.ab[0],
		session_info->keyExchangeKey.ab[8],
		session_info->hostAddress.ina,
		session_info->hostAddress.inaOnline,
		session_info->hostAddress.wPortOnline,
		session_info->hostAddress.abEnet[0], 
		session_info->hostAddress.abEnet[4],
		session_info->hostAddress.abOnline[0],
		session_info->hostAddress.abOnline[8],
		session_info->hostAddress.abOnline[16]);
}

void discord_rich_presence_update(s_discord_data* discord)
{
	g_discord_activity.application_id = DISCORD_CLIENT_ID;
	g_discord_activity.supported_platforms = DiscordActivitySupportedPlatformFlags_Desktop;
	
	s_network_session* session = nullptr;
	if (NetworkSession::GetActiveNetworkSession(&session))
	{
		XSESSION_INFO* session_info = global_xsession_manager_get()->get_session();
		XUID host;
		XUserGetXUID(0, &host);

		snprintf(g_discord_activity.party.id, sizeof(g_discord_activity.party.id), "%016llx", host);
		g_discord_activity.party.privacy = DiscordActivityPartyPrivacy_Public;
		discord_interface_encode_xsession_info(session_info);

		LOG_INFO_FUNC("Join secret:");
		for (uint32 i = 0; i < 128; ++i)
			LOG_INFO_FUNC("{}", g_discord_activity.secrets.join[i]);
	}
	else
	{
		memset(g_discord_activity.secrets.join, 0, sizeof(DiscordActivitySecrets::join));
	}


	discord->activities->update_activity(discord->activities, &g_discord_activity, discord, on_rich_presence_updated);
	return;
}

uint32 discord_update(s_discord_data* discord)
{
	discord->core->run_callbacks(discord->core);

	if (g_update_rich_presence)
	{
		discord_rich_presence_update(discord);
		g_update_rich_presence = false;
	}

	return 50;
}

void DISCORD_CALLBACK on_user_updated(void* data)
{
	s_discord_data* discord = (s_discord_data*)data;
	DiscordUser user;
	discord->users->get_current_user(discord->users, &user);
	discord->user_id = user.id;
	return;
}

void DISCORD_CALLBACK on_activity_join(void* event_data, const char* secret)
{

	for (uint32 i = 0; i < 128; ++i)
		LOG_INFO_FUNC("{}", secret[i]);

	XSESSION_INFO session = {};
	sscanf(secret, 
		"%llu/%llu/%llu/%lu/%lu/%hu/%lu/%hu/%llu/%llu/%lu",
		&session.sessionID,
		&session.keyExchangeKey.ab[0],
		&session.keyExchangeKey.ab[8],
		&session.hostAddress.ina,
		&session.hostAddress.inaOnline,
		&session.hostAddress.wPortOnline,
		&session.hostAddress.abEnet[0],
		&session.hostAddress.abEnet[4],
		&session.hostAddress.abOnline[0],
		&session.hostAddress.abOnline[8],
		&session.hostAddress.abOnline[16]);

	c_user_interface_guide_state_manager* manager = user_interface_guide_state_manager_get();
	memcpy(&manager->m_xsession_info, &session, sizeof(XSESSION_INFO));
	manager->m_from_game_invite = 0;
	manager->m_unk_bool_D = 0;

	user_interface_networking_join_game(&manager->m_xsession_info, 0, true);
	return;
}

void DISCORD_CALLBACK on_discord_log_print(void* hook_data, enum EDiscordLogLevel level, const char* message)
{
	LOG_INFO_FUNC("{}", message);
	return;
}

DWORD WINAPI discord_thread_proc(LPVOID lpParam)
{
	SetEnvironmentVariable(L"DISCORD_INSTANCE_ID", L"0");
	uint32 result = 0;

	IDiscordUserEvents users_events = {};
	IDiscordActivityEvents activities_events = {};
	s_discord_data discord = {};

	users_events.on_current_user_update = on_user_updated;
	activities_events.on_activity_join = on_activity_join;

	DiscordCreateParams params = {};
	DiscordCreateParamsSetDefault(&params);
	params.client_id = DISCORD_CLIENT_ID;
	params.flags = DiscordCreateFlags_NoRequireDiscord;
	params.event_data = &discord;
	params.user_events = &users_events;
	params.activity_events = &activities_events;
	EDiscordResult res = DiscordCreate(DISCORD_VERSION, &params, &discord.core);

	if (res == DiscordResult_Ok)
	{
		discord.core->set_log_hook(discord.core, DiscordLogLevel_Debug, &discord, on_discord_log_print);
		discord.users = discord.core->get_user_manager(discord.core);
		discord.activities = discord.core->get_activity_manager(discord.core);
		discord.lobbies = discord.core->get_lobby_manager(discord.core);
		discord.activities->register_command(discord.activities, "halo2.exe");

		// Main loop
		while (!g_should_stop)
		{
			Sleep(discord_update(&discord));
		}

		// Cleanup
		discord.activities->clear_activity(discord.activities, &discord, on_rich_presence_updated);
		discord.core->destroy(discord.core);
	}
	else
	{
		LOG_ERROR_FUNC("Failed to create Discord Game SDK instance ({})\n", res);
		result = 1;
	}

	return result;
}

void discord_game_status_create(void)
{
	discord_thread = CreateThread(NULL, 0, discord_thread_proc, NULL, 0, NULL);
	return;
}

void discord_game_status_dispose(void)
{
	InterlockedAdd(&g_should_stop, 1);
	WaitForSingleObject(discord_thread, INFINITE);
	return;
}