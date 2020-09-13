#include "stdafx.h"
#include "Globals.h"

//all these global variables live till the app closes
AdvLobbySettings* advLobbySettings = new AdvLobbySettings();

s_datum_array* game_state_actors = nullptr;
s_datum_array* game_state_objects_header = nullptr;

bool displayXyz = false;
