
#include "stdafx.h"

#include "cartographer.h"

#include "game/game.h"

/* public code */
bool cartographer_allow_action_during_networked_game()
{
#if defined(RELEASE_DLL) && RELEASE_DLL
	return !game_is_networked() && (game_is_campaign() || game_is_ui_shell());
#else
	return true;
#endif
}