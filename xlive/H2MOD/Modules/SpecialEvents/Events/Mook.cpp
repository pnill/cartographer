#include "stdafx.h"

#include "Mook.h"
#include "../SpecialEventHelpers.h"

#include "items/weapon_definitions.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"


void mook_event_map_load()
{
	datum ball_weapon_datum = tags::find_tag(_tag_group_weapon, "objects\\weapons\\multiplayer\\ball\\ball");
	datum bomb_weapon_datum = tags::find_tag(_tag_group_weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
	datum mook_ball_weapon_datum = tag_loader::Get_tag_datum ("scenarios\\objects\\multi\\carto_shared\\basketball\\basketball", _tag_group_weapon, "carto_shared");

	if (!DATUM_IS_NONE(mook_ball_weapon_datum) && !DATUM_IS_NONE(ball_weapon_datum) && !DATUM_IS_NONE(bomb_weapon_datum))
	{
		tag_loader::Load_tag(mook_ball_weapon_datum, true, "carto_shared");
		tag_loader::Push_Back();

		mook_ball_weapon_datum = tag_loader::ResolveNewDatum(mook_ball_weapon_datum);

		auto mook_ball_weapon = tags::get_tag<_tag_group_weapon, _weapon_definition>(mook_ball_weapon_datum, true);

		replace_fp_and_3p_models_from_weapon(ball_weapon_datum, mook_ball_weapon->item.object.model.index, mook_ball_weapon->item.object.model.index);
		replace_fp_and_3p_models_from_weapon(bomb_weapon_datum, mook_ball_weapon->item.object.model.index, mook_ball_weapon->item.object.model.index);
	}
}
