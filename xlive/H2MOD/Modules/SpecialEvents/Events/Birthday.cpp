#include "stdafx.h"

#include "Birthday.h"
#include "../SpecialEventHelpers.h"

#include "Blam/Cache/TagGroups/weapon_definition.h"
#include "Blam/Engine/Game/GameGlobals.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"

void birthday_event_map_load()
{
	// Carto Shared Tags
	datum bday_hat_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\birthday_hat\\birthday_hat", blam_tag::tag_group_type::scenery, "carto_shared");
	datum bday_cake_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\birthday_cake\\birthday_cake", blam_tag::tag_group_type::rendermodel, "carto_shared");
	datum fp_bday_cake_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\birthday_cake\\fp\\fp", blam_tag::tag_group_type::rendermodel, "carto_shared");

	// Halo 2 Tags
	datum ball_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\ball\\ball");
	datum bomb_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");

	if (!DATUM_IS_NONE(bday_hat_datum))
	{
		tag_loader::Load_tag(bday_hat_datum, true, "carto_shared");
		tag_loader::Push_Back();

		bday_hat_datum = tag_loader::ResolveNewDatum(bday_hat_datum);

		// Give Birthday Hat and Beard to Chief & Friends
		if (datum hlmt_chief_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief");
			hlmt_chief_datum != DATUM_INDEX_NONE) 
		{
			add_hat_to_model(hlmt_chief_datum, bday_hat_datum);
		}
		if (datum hlmt_chief_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp");
			hlmt_chief_mp_datum != DATUM_INDEX_NONE) 
		{
			add_hat_to_model(hlmt_chief_mp_datum, bday_hat_datum);
		}
		datum hlmt_elite_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite_mp");
		if (hlmt_elite_datum != DATUM_INDEX_NONE)
		{
			add_hat_to_model(hlmt_elite_datum, bday_hat_datum, true);
		}
	}

	if (!DATUM_IS_NONE(bday_cake_datum) && !DATUM_IS_NONE(fp_bday_cake_datum) && !DATUM_IS_NONE(ball_weapon_datum) && !DATUM_IS_NONE(bomb_weapon_datum))
	{
		tag_loader::Load_tag(bday_cake_datum, true, "carto_shared");
		tag_loader::Load_tag(fp_bday_cake_datum, true, "carto_shared");
		tag_loader::Push_Back();

		bday_cake_datum = tag_loader::ResolveNewDatum(bday_cake_datum);
		fp_bday_cake_datum = tag_loader::ResolveNewDatum(fp_bday_cake_datum);

		replace_fp_and_3p_models_from_weapon(ball_weapon_datum, fp_bday_cake_datum, bday_cake_datum);
		replace_fp_and_3p_models_from_weapon(bomb_weapon_datum, fp_bday_cake_datum, bday_cake_datum);
	}
}
