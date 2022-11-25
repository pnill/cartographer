#include "stdafx.h"

#include "SpecialEvents.h"
#include "SpecialEventHelpers.h"
#include "Events/Birthday.h"
#include "Events/Christmas.h"
#include "Events/Halloween.h"
#include "Events/Mook.h"
#include "Events/Paddy.h"

#include "Blam/Engine/Game/networking/messages/network_message_type_collection.h"
#include "H2MOD/GUI/imgui_integration/imgui_handler.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"

namespace SpecialEvents
{
	// This function gets the current date and time
	std::time_t getEpochTime(int year, const std::wstring& dateTime)
		{
			// Let's consider we are getting all the input in
			// this format: '2014-07-25T20:17:22Z' (T denotes
			// start of Time part, Z denotes UTC zone).
			// A better approach would be to pass in the format as well.
			static const std::wstring dateTimeFormat{ L"%Y-%m-%dT%H:%M:%SZ" };

			// Create a stream which we will use to parse the string,
			// which we provide to constructor of stream to fill the buffer.
			std::wstring tmp = std::to_wstring(year + 1900);
			tmp.append(L"-");
			tmp.append(dateTime);
			std::wistringstream ss{ tmp };

			// Create a tm object to store the parsed date and time.
			std::tm dt;

			// Now we read from buffer using get_time manipulator
			// and formatting the input appropriately.
			ss >> std::get_time(&dt, dateTimeFormat.c_str());

			// Convert the tm structure to time_t value and return.
			return std::mktime(&dt);
		}
	
	// This function checks if the current date passed is in the current week.
	bool CheckEventWeek(std::wstring date)
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			time_t tt = std::chrono::system_clock::to_time_t(now);
			tm utc_tm = *gmtime(&tt);

			time_t pat = getEpochTime(utc_tm.tm_year, date.append(L"T00:00:00Z"));
			tm utc_pat = *gmtime(&pat);

			int a, b = 0;
			a = utc_tm.tm_yday - utc_tm.tm_wday;
			b = utc_pat.tm_yday - utc_pat.tm_wday;
			return a == b;
		}
	
	// This function checks if the current date matches the one passed.
	bool CheckEventDate(std::wstring date)
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			time_t tt = std::chrono::system_clock::to_time_t(now);
			tm utc_tm = *gmtime(&tt);

			time_t pat = getEpochTime(utc_tm.tm_year, date.append(L"T00:00:00Z"));
			tm utc_pat = *gmtime(&pat);

			return utc_tm.tm_yday == utc_pat.tm_yday;
		}

	// Enables event if the current date and time line up with an event time
	e_event_type getCurrentEvent()
	{
		if (H2Config_no_events)
			return _no_event;

#ifndef NDEBUG
		if (H2Config_forced_event != _no_event)
			return (e_event_type)H2Config_forced_event;
#endif
		if (CheckEventWeek(L"3-17"))
			return _st_paddys;

		if (CheckEventWeek(L"12-24") || CheckEventWeek(L"12-30") || CheckEventWeek(L"1-4"))
			return _christmas;

		// One time event
		/*if (CheckIfEventTime(L"4-12"))
			return _mook_maddness;*/

		if (CheckEventWeek(L"10-20") || CheckEventWeek(L"10-27") || CheckEventDate(L"10-31"))
			return _halloween;

		if (CheckEventDate(L"11-08") || CheckEventDate(L"11-09") || CheckEventDate(L"11-10"))
			return _birthday;

		return _no_event;
	}

	void Initialize()
	{
		if (tag_loader::Map_exists("carto_shared"))
		{
			tags::on_map_load(AddNewMarkers);
			switch (getCurrentEvent())
			{
			case _christmas:
				tags::on_map_load(ChristmasOnMapLoad);
				break;
			case _st_paddys:
				tags::on_map_load(PaddysOnMapLoad);
				break;
			case _mook_maddness:
				tags::on_map_load(MookMaddnessOnMapLoad);
				break;
			case _halloween:
				tags::on_map_load(HalloweenOnMapLoad);
				break;
			case _birthday:
				tags::on_map_load(BirthdayOnMapLoad);
				break;
			}
		}
		else
		{
			if (!NetworkSession::LocalPeerIsSessionHost())
			{
				*Memory::GetAddress<int*>(0x46DCF1) = 1;
				ImGuiHandler::ImMessageBox::SetMessage("Error: Cartographer Shared map content is missing. Try updating your game from the mainmenu.\r\n\r\nBy going to Cartographer > Update.\r\n\r\nIf that doesn't work reach out to us in #help on discord.");
				ImGuiHandler::ToggleWindow(ImGuiHandler::ImMessageBox::windowName);
			}
		}
	}
}
