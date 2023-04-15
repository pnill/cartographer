#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"
#include "Blam/Engine/tag_files/string_id.h"
#include "Blam\Cache\TagGroups\globals_definition.hpp"
#include "Blam/Engine/game/players.h"

namespace PlayerRepresentation
{
	/**
	 * \brief Adds a new player representation to the globals tag block
	 * \param fp_hands the datum index of the first person hands render model
	 * \param fp_body the datum index of the first person body render model
	 * \param tp_biped the datum index of the third person biped
	 * \param type the type of biped being added, will override an existing biped
	 * \param variant the string_id of the variant
	 * \return returns a pointer to the new representation
	 */
	s_globals_group_definition::s_player_representation_block* add_representation(datum fp_hands, datum fp_body, datum tp_biped, s_player::e_character_type type, string_id variant = -1);
	/**
	 * \brief Clones an existing player_representation
	 * \param index 0 based index of the representation to clone
	 * \param newType the new Player::Biped type to register this as, cannot be the same as an existing one.
	 * \return returns a pointer to the new representation
	 */
	s_globals_group_definition::s_player_representation_block* clone_representation(int index, s_player::e_character_type newType);

	datum get_object_datum_from_representation(s_player::e_character_type representation_index);
	void Initialize();
}
