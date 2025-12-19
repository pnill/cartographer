#pragma once

/* globals */

extern bool g_instance_is_live;
extern bool g_instance_is_lan;

/* prototypes */

void kablam_shell_initialize(void);

bool kablam_shell_argument_exists(const wchar_t* argument);

int kablam_shell_argument_get_index(const wchar_t* argument);

wchar_t* kablam_shell_argument_get_value(const wchar_t* argument);

bool kablam_shell_read_input(wchar_t* out_buffer, size_t max_read_size);

bool kablam_shell_read_input_failed(void);
