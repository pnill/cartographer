#include "stdafx.h"

#include "Birthday.h"
#include "../SpecialEventHelpers.h"

#include "items/weapon_definitions.h"
#include "game/game_globals.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "shaders/shader_definitions.h"
#include "tag_files/tag_loader/tag_injection.h"

void birthday_event_map_load()
{
	tag_injection_set_active_map(k_events_map);
	if (tag_injection_active_map_verified())
	{
		datum hat_datum = tag_injection_load(_tag_group_scenery, "scenarios\\objects\\multi\\carto_shared\\birthday_hat\\birthday_hat", true);
		datum ball_weapon_datum = tag_loaded(_tag_group_weapon, "objects\\weapons\\multiplayer\\ball\\ball");
		datum bomb_weapon_datum = tag_loaded(_tag_group_weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
		datum bday_cake_datum = tag_injection_load(_tag_group_render_model, "scenarios\\objects\\multi\\carto_shared\\birthday_cake\\birthday_cake", true);
		datum fp_bday_cake_datum = tag_injection_load(_tag_group_render_model, "scenarios\\objects\\multi\\carto_shared\\birthday_cake\\fp\\fp", true);

		if (hat_datum != NONE && bday_cake_datum != NONE && fp_bday_cake_datum != NONE && ball_weapon_datum != NONE && bomb_weapon_datum != NONE)
		{
			tag_injection_inject();

			if (datum hlmt_chief_datum = tag_loaded(_tag_group_model, "objects\\characters\\masterchief\\masterchief");
				hlmt_chief_datum != NONE)
			{
				add_hat_to_model(hlmt_chief_datum, hat_datum);
			}
			if (datum hlmt_chief_mp_datum = tag_loaded(_tag_group_model, "objects\\characters\\masterchief\\masterchief_mp");
				hlmt_chief_mp_datum != NONE)
			{
				add_hat_to_model(hlmt_chief_mp_datum, hat_datum);
			}
			datum hlmt_elite_datum = tag_loaded(_tag_group_model, "objects\\characters\\elite\\elite_mp");
			if (hlmt_elite_datum != NONE)
			{
				add_hat_to_model(hlmt_elite_datum, hat_datum, true);
			}

			replace_fp_and_3p_models_from_weapon(ball_weapon_datum, fp_bday_cake_datum, bday_cake_datum);
			replace_fp_and_3p_models_from_weapon(bomb_weapon_datum, fp_bday_cake_datum, bday_cake_datum);
		}
	}
}
