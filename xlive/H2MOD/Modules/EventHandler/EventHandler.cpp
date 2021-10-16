
#include "EventHandler.hpp"

std::map<EventType, std::vector<EventCallback>> event_map =
{
	{ EventType::network_player,	{} },
	{ EventType::gamelifecycle_change,	{} },
	{ EventType::game_loop,			{} },
	{ EventType::server_command,	{} },
	{ EventType::map_load,			{} },
	{ EventType::countdown_start,	{} },
	{ EventType::player_control,	{} },
	{ EventType::blue_screen,		{} },
	{ EventType::player_spawn,		{} },
	{ EventType::player_death,		{} }
};