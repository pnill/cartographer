#include "stdafx.h"

#include "Mook.h"
#include "../SpecialEventHelpers.h"

#include "items/weapon_definitions.h"
#include "game/game_globals.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "tag_files/tag_loader/tag_injection.h"


void mook_event_map_load()
{
	datum ball_weapon_datum = tags::find_tag(_tag_group_weapon, "objects\\weapons\\multiplayer\\ball\\ball");
	datum bomb_weapon_datum = tags::find_tag(_tag_group_weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");

	tag_injection_set_active_map(k_events_map);
	if (!tag_injection_active_map_verified())
		return;

	datum mook_ball_weapon_datum = tag_injection_load(_tag_group_weapon, "scenarios\\objects\\multi\\carto_shared\\basketball\\basketball", true);

	if (!DATUM_IS_NONE(mook_ball_weapon_datum) && !DATUM_IS_NONE(ball_weapon_datum) && !DATUM_IS_NONE(bomb_weapon_datum))
	{
		tag_injection_inject();

		auto mook_ball_weapon = tags::get_tag<_tag_group_weapon, _weapon_definition>(mook_ball_weapon_datum, true);

		replace_fp_and_3p_models_from_weapon(ball_weapon_datum, mook_ball_weapon->item.object.model.index, mook_ball_weapon->item.object.model.index);
		replace_fp_and_3p_models_from_weapon(bomb_weapon_datum, mook_ball_weapon->item.object.model.index, mook_ball_weapon->item.object.model.index);
	}
}
