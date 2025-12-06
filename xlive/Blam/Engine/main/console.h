#pragma once

/* prototypes */

#ifdef TERMINAL_ENABLED

void console_initialize(void);

void console_dispose(void);

bool console_is_active(void);

void console_execute_initial_commands(void);

void console_open(bool open_debug_menu);

void console_close(void);

void console_clear(void);

void console_printf(const char* format, ...);

void console_warning(const char* format, ...);

void console_update(real32 dt);

#else

#define console_initialize() (void)0 

#define console_dispose() (void)0

#define console_is_active() (void)0

#define console_execute_initial_commands() (void)0

#define console_open(...) (void)0

#define console_close() (void)0

#define console_clear() (void)0

#define console_printf(...) (void)0

#define console_warning(...) (void)0

#define console_update(...) (void)0

#endif




