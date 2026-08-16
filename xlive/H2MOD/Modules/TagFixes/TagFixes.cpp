#include "stdafx.h"
#include "TagFixes.h"

#include "cache/cache_files.h"
#include "objects/light_definitions.h"
#include "shaders/shader_definitions.h"
#include "shell/shell.h"
#include "units/biped_definitions.h"

#include "H2MOD/Modules/Shell/Config.h"

// ### TODO Cleanup
extern bool g_xbox_tickrate_enabled;

namespace TagFixes
{
	namespace
	{
		void font_table_fix()
		{
			WriteValue<int>(Memory::GetAddress(0x464940), 0);
			WriteValue<int>(Memory::GetAddress(0x46494C), 0);
			WriteValue<int>(Memory::GetAddress(0x464958), 0);
			WriteValue<int>(Memory::GetAddress(0x464964), 0);
		}

		void shader_lod_max()
		{
			tag_iterator iterator;
			tag_iterator_new(&iterator, _tag_group_shader);

			while (tag_iterator_next(&iterator) != NONE)
			{
				s_shader_definition* shader = shader_definition_get(iterator.current_tag_index);
				shader->shader_lod_bias = _shader_lod_bias_never;
			}
		}

		void light_framerate_killer()
		{
			tag_iterator iterator;
			tag_iterator_new(&iterator, _tag_group_light);

			while (tag_iterator_next(&iterator) != NONE)
			{
				light_definition* light = light_definition_get(iterator.current_tag_index);
				// Disabled since it caused issues where certain lights wouldnt render randomly
				// TODO figure out why it does this at some other point in time
				// light->flags.set(_light_definition_light_framerate_killer, true);

				light->flags.set(_light_definition_multiplayer_override, true);
			}
		}

		void fall_damage_fix()
		{
			tag_iterator iterator;
			tag_iterator_new(&iterator, _tag_group_biped);

			while (tag_iterator_next(&iterator) != NONE)
			{
				struct biped_definition* biped_definition = biped_definition_get(iterator.current_tag_index);

				biped_definition->object.abs_acceleration.lower *= 2.f;
				biped_definition->object.abs_acceleration.upper *= 2.f;
			}
		}
	}

	void OnMapLoad()
	{
		if (!shell_is_dedicated_server()) 
		{
			font_table_fix();
			if (H2Config_shader_lod_max)
			{
				shader_lod_max();
			}
			if (H2Config_light_suppressor_disable)
			{
				light_framerate_killer();
			}
		}
		if (!g_xbox_tickrate_enabled)
		{
			// both client/server
			fall_damage_fix();
		}
	}
}
