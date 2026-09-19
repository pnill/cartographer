#pragma once

/* macros */

#define widget_get(index) ((struct widget_datum*)datum_get(widget_data_get(), (index)))

/* enums */

enum e_widget_type : int16
{
	_widget_type_antenna,
	_widget_type_cloth,
	_widget_type_liquid,
	k_number_of_widget_types
};

/* structures */

struct widget_datum
{
	int16 identifier;
	e_widget_type type;
	datum type_datum_index;
	datum next_widget_index;
};
ASSERT_STRUCT_SIZE(widget_datum, 12);

struct widget_type_definition
{
	tag_group group_tag;
	uint32 flags; // unused
	void(__cdecl* initialize)(void);
	void(__cdecl* initialize_for_new_map)(void);
	void(__cdecl* dispose_from_old_map)(void);
	void(__cdecl* dispose)(void);
	datum(__cdecl* new_proc)(datum definition_index, datum object_index);
	void(__cdecl* delete_proc)(datum type_datum_index);
	void(__cdecl* update)(real32 dt);
	void(__cdecl* render_debug_proc)(datum object_index, datum type_datum_index);
	void(__cdecl* render_proc)(datum unused_shader_index, int32 unused_1, int32 unused_2, int32 unused_3, int32 unused_4, datum cloth_index, int8* unused_out_buffer);
	void(__cdecl* transparent_render_proc)(void* payload);
	datum(__cdecl* get_shader_proc)(datum type_datum_index);
	void(__cdecl* render_transparent_proc)(int16 layer, datum type_datum_index);
};
ASSERT_STRUCT_SIZE(widget_type_definition, 56);

/* prototypes */

widget_type_definition* widget_type_definition_get(e_widget_type type);

struct data_array* widget_data_get(void);

void __cdecl widgets_initialize_for_new_map(void);

void __cdecl widgets_new(datum object_index);

void __cdecl widgets_update(real32 dt);

void __cdecl widget_delete_type_datum(datum object_index, datum delete_widget_type_index);
