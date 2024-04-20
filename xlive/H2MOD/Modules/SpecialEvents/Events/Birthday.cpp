#include "stdafx.h"

#include "Birthday.h"
#include "../SpecialEventHelpers.h"

#include "items/weapon_definitions.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"

void birthday_event_map_load()
{
	// Carto Shared Tags
	datum bday_hat_datum = tag_loader::get_tag_datum_by_name("scenarios\\objects\\multi\\carto_shared\\birthday_hat\\birthday_hat", _tag_group_scenery, "carto_shared");
	datum bday_cake_datum = tag_loader::get_tag_datum_by_name("scenarios\\objects\\multi\\carto_shared\\birthday_cake\\birthday_cake", _tag_group_render_model, "carto_shared");
	datum fp_bday_cake_datum = tag_loader::get_tag_datum_by_name("scenarios\\objects\\multi\\carto_shared\\birthday_cake\\fp\\fp", _tag_group_render_model, "carto_shared");

	// Halo 2 Tags
	datum ball_weapon_datum = tags::find_tag(_tag_group_weapon, "objects\\weapons\\multiplayer\\ball\\ball");
	datum bomb_weapon_datum = tags::find_tag(_tag_group_weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");

	if (!DATUM_IS_NONE(bday_hat_datum))
	{
		tag_loader::preload_tag_data_from_cache(bday_hat_datum, true, "carto_shared");
		tag_loader::push_loaded_tag_data();

		bday_hat_datum = tag_loader::resolve_cache_index_to_injected(bday_hat_datum);

		// Give Birthday Hat and Beard to Chief & Friends
		if (datum hlmt_chief_datum = tags::find_tag(_tag_group_model, "objects\\characters\\masterchief\\masterchief");
			hlmt_chief_datum != NONE) 
		{
			add_hat_to_model(hlmt_chief_datum, bday_hat_datum);
		}
		if (datum hlmt_chief_mp_datum = tags::find_tag(_tag_group_model, "objects\\characters\\masterchief\\masterchief_mp");
			hlmt_chief_mp_datum != NONE) 
		{
			add_hat_to_model(hlmt_chief_mp_datum, bday_hat_datum);
		}
		datum hlmt_elite_datum = tags::find_tag(_tag_group_model, "objects\\characters\\elite\\elite_mp");
		if (hlmt_elite_datum != NONE)
		{
			add_hat_to_model(hlmt_elite_datum, bday_hat_datum, true);
		}
	}

	if (!DATUM_IS_NONE(bday_cake_datum) && !DATUM_IS_NONE(fp_bday_cake_datum) && !DATUM_IS_NONE(ball_weapon_datum) && !DATUM_IS_NONE(bomb_weapon_datum))
	{
		tag_loader::preload_tag_data_from_cache(bday_cake_datum, true, "carto_shared");
		tag_loader::preload_tag_data_from_cache(fp_bday_cake_datum, true, "carto_shared");
		tag_loader::push_loaded_tag_data();

		bday_cake_datum = tag_loader::resolve_cache_index_to_injected(bday_cake_datum);
		fp_bday_cake_datum = tag_loader::resolve_cache_index_to_injected(fp_bday_cake_datum);

		replace_fp_and_3p_models_from_weapon(ball_weapon_datum, fp_bday_cake_datum, bday_cake_datum);
		replace_fp_and_3p_models_from_weapon(bomb_weapon_datum, fp_bday_cake_datum, bday_cake_datum);
	}
}
