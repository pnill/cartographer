#pragma once

namespace MeleeFix
{
	struct alignas(4) s_melee_animation_datum
	{
		DWORD field_0;
		DWORD melee_flags;
		BYTE gap_8[36];
		int melee_type_string_id;
		char field_30;
		char field_31;
		BYTE melee_animation_update;
		BYTE animation_action_index;
		char unk;
		BYTE max_animation_range;
	};

	void MeleeCollisionPatch();
	void ApplyHooks();
	void Initialize();
}
