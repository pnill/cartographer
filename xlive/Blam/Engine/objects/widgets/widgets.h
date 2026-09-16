#pragma once

void __cdecl widgets_initialize_for_new_map(void);

void __cdecl widgets_new(datum object_index);

void __cdecl widgets_update(real32 dt);

void __cdecl widget_delete_type_datum(datum object_index, datum widget_index);
