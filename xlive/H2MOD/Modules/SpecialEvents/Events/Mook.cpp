#include "stdafx.h"

#include "Mook.h"
#include "../SpecialEventHelpers.h"

#include "Blam/Cache/TagGroups/weapon_definition.hpp"
#include "Blam/Engine/game/game_globals.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"


void mook_event_map_load()
{
	datum ball_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\ball\\ball");
	datum bomb_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
	datum mook_ball_weapon_datum = tag_loader::Get_tag_datum ("scenarios\\objects\\multi\\carto_shared\\basketball\\basketball", blam_tag::tag_group_type::weapon, "carto_shared");

	if (!DATUM_IS_NONE(mook_ball_weapon_datum) && !DATUM_IS_NONE(ball_weapon_datum) && !DATUM_IS_NONE(bomb_weapon_datum))
	{
		tag_loader::Load_tag(mook_ball_weapon_datum, true, "carto_shared");
		tag_loader::Push_Back();

		mook_ball_weapon_datum = tag_loader::ResolveNewDatum(mook_ball_weapon_datum);

		auto mook_ball_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, s_weapon_group_definition>(mook_ball_weapon_datum, true);

		replace_fp_and_3p_models_from_weapon(ball_weapon_datum, mook_ball_weapon->model.TagIndex, mook_ball_weapon->model.TagIndex);
		replace_fp_and_3p_models_from_weapon(bomb_weapon_datum, mook_ball_weapon->model.TagIndex, mook_ball_weapon->model.TagIndex);
	}
}
