#pragma once

/* prototypes */

void kablam_shell_initialize(void);

bool kablam_shell_argument_exists(wchar_t const* argument);

int32 kablam_shell_argument_get_index(wchar_t const* argument);

wchar_t* kablam_shell_argument_get_value(wchar_t const* argument);

bool kablam_shell_read_input(wchar_t* out_buffer, size_t max_read_size);

bool kablam_shell_read_input_failed(void);

/* globals */

extern bool g_instance_is_live;
extern bool g_instance_is_lan;
