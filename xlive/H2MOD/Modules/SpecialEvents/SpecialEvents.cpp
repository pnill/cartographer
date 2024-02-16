#include "stdafx.h"
#include "SpecialEvents.h"
#include "SpecialEventHelpers.h"
#include "Events/Birthday.h"
#include "Events/Christmas.h"
#include "Events/Halloween.h"
#include "Events/Mook.h"
#include "Events/Paddy.h"

#include "game/game.h"
#include "Networking/NetworkMessageTypeCollection.h"

#include "H2MOD/GUI/imgui_integration/imgui_handler.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"

// Enables event if the current date and time line up with an event time
e_special_event_type get_current_special_event()
{
	e_special_event_type event = _special_event_none;

	if (H2Config_no_events)
		return event;

	s_date_and_time date;
	game_time_get_date_and_time(&date);

#ifndef NDEBUG
	if (H2Config_forced_event != _special_event_none)
		return (e_special_event_type)H2Config_forced_event;
#endif

	switch (date.month)
	{
	// January
	case 1:
		event = (IN_RANGE_INCLUSIVE(date.day, 1, 7) ? _special_event_christmas : event);
		break;
	// March
	case 3:
		event = (IN_RANGE_INCLUSIVE(date.day, 17, 24) ? _special_event_st_paddys : event);
		break;
	// April
	/* One time event
	case 4:
		event = (date.day == 12 ? _special_event_mook_maddness : event);
		break;
	*/
	// May
	case 5:
		event = (IN_RANGE_INCLUSIVE(date.day, 30, 31) ? _special_event_birthday : event);
		break;
	// October
	case 10:
		event = (IN_RANGE_INCLUSIVE(date.day, 17, 31) ? _special_event_halloween : event);
		break;
	// November
	case 11:
		event = (IN_RANGE_INCLUSIVE(date.day, 8, 10) ? _special_event_birthday : event);
		break;
	// December
	case 12:
		event = (IN_RANGE_INCLUSIVE(date.day, 20, 31) ? _special_event_christmas : event);
		break;
	}

	return event;
}

void load_special_event()
{
	if (tag_loader::Map_exists("carto_shared"))
	{
		bool create_new_markers = true;
		switch (get_current_special_event())
		{
		case _special_event_christmas:
			christmas_event_map_load();
			break;
		case _special_event_st_paddys:
			paddy_event_map_load();
			break;
		case _special_event_mook_maddness:
			mook_event_map_load();
			break;
		case _special_event_halloween:
			halloween_event_map_load();
			break;
		case _special_event_birthday:
			birthday_event_map_load();
			break;
		default:
			create_new_markers = false;
			break;
		}
		
		if (create_new_markers)
		{
			add_special_event_markers();
		}
	}
	else
	{
		if (!NetworkSession::LocalPeerIsSessionHost())
		{
			*Memory::GetAddress<byte*>(0x46DCF1) = 1;
			ImGuiHandler::ImMessageBox::SetMessage("Error: Cartographer Shared map content is missing. Try updating your game from the mainmenu.\r\n\r\nBy going to Cartographer > Update.\r\n\r\nIf that doesn't work reach out to us in #help on discord.");
			ImGuiHandler::ToggleWindow(ImGuiHandler::ImMessageBox::windowName);
		}
	}
}

