#include "stdafx.h"
#include "kablam_shell.h"

/* constants */

enum
{
    kablam_shell_max_arguments = 256,
};

/* prototypes */

static void kablam_shell_parse_command_line(void);

/* globals */

static wchar_t g_kablam_shell_command_line[SHORT_MAX + 1]{};

static wchar_t* g_kablam_shell_arguments[kablam_shell_max_arguments]{};

static int32 g_kablam_shell_arguments_count = 0;

static bool g_kablam_shell_failed_to_read_input = false;

bool g_instance_is_live = false;
bool g_instance_is_lan = false;

/* public code */

void kablam_shell_initialize(void)
{
	wcsncpy_s(g_kablam_shell_command_line, NUMBEROF(g_kablam_shell_command_line)-1, GetCommandLineW(), UINT_MAX);
    kablam_shell_parse_command_line();
    return;
}

bool kablam_shell_argument_exists(
    wchar_t const* argument)
{
    return kablam_shell_argument_get_index(argument) != -1;
}

int32 kablam_shell_argument_get_index(
    wchar_t const* argument)
{
    if (g_kablam_shell_arguments_count <= 1 || argument == nullptr)
        return -1;

    for (int i = 1; i < g_kablam_shell_arguments_count; ++i)
    {
        wchar_t const* current_argument = g_kablam_shell_arguments[i];
        if (current_argument != nullptr && _wcsicmp(current_argument, argument) == 0)
            return i;
    }

    return -1;
}

wchar_t* kablam_shell_argument_get_value(
    wchar_t const* argument)
{
    wchar_t* result = nullptr;

    if (argument && g_kablam_shell_arguments_count > 1)
    {
        size_t argument_length = wcslen(argument);

        for (int i = 1; i < g_kablam_shell_arguments_count; ++i)
        {
            wchar_t* current_argument = g_kablam_shell_arguments[i];

            if (!current_argument)
            {
                continue;
            }

            if (_wcsnicmp(current_argument, argument, argument_length) == 0)
            {
                result = current_argument + argument_length;
                break;
            }
        }
    }

    return result;
}

bool kablam_shell_read_input(
    wchar_t* out_buffer,
    size_t max_read_size)
{
    bool result = false;

    if (!g_kablam_shell_failed_to_read_input)
    {
        if (_getws_s(out_buffer, max_read_size))
        {
            result = (wcslen(out_buffer) < max_read_size - 1);
        }
        else
        {
            g_kablam_shell_failed_to_read_input = true;
        }
    }

    return result;
}

bool kablam_shell_read_input_failed(void)
{
    return g_kablam_shell_failed_to_read_input;
}

/* private code */

static void kablam_shell_parse_command_line(void)
{
    // write pointer used when compacting quoted strings
    wchar_t* quote_write_ptr = NULL;

    // number of arguments found
    int argument_count = 0;

    // parser state:
    // 0 = scanning for start of next argument
    // 1 = inside unquoted argument
    // 2 = inside quoted section, before compaction
    // 3 = just after a closing quote, deciding what it means
    // 4 = copying/compacting characters after quote
    int state = 0;

    // points at the last filled slot in launch_argument_storage
    wchar_t** current_arg_slot = g_kablam_shell_arguments - 1;

    wchar_t* startup_command_line = g_kablam_shell_command_line;

    for (;;)
    {
        switch (state)
        {
        case 0: // scanning for next argument
        {
            wchar_t ch = *startup_command_line;

            switch (ch)
            {
            case L'\0':
                g_kablam_shell_arguments_count = argument_count;
                return;

            case L'\t':
            case L' ':
                // skip whitespace
                ++startup_command_line;
                break;

            case L'"':
                // quoted argument start
                ++startup_command_line;
                state = 2;

                ++argument_count;
                ++current_arg_slot;
                if (argument_count >= kablam_shell_max_arguments)
                {
                    g_kablam_shell_arguments_count = argument_count;
                    return;
                }
                *current_arg_slot = startup_command_line;
                break;

            default:
                // unquoted argument start
                state = 1;

                ++argument_count;
                ++current_arg_slot;
                if (argument_count >= kablam_shell_max_arguments)
                {
                    g_kablam_shell_arguments_count = argument_count;
                    return;
                }
                *current_arg_slot = startup_command_line;
                break;
            }
            break;
        }

        case 1: // inside unquoted argument
        {
            wchar_t ch = *startup_command_line;

            if (ch == L'\0')
            {
                g_kablam_shell_arguments_count = argument_count;
                return;
            }

            if (ch == L'\t' || ch == L' ')
            {
                // terminate current argument
                *startup_command_line++ = L'\0';
                state = 0;
            }
            else
            {
                ++startup_command_line;
            }
            break;
        }

        case 2: // inside quoted section, before compaction
            if (*startup_command_line == L'\0')
            {
                g_kablam_shell_arguments_count = argument_count;
                return;
            }

            if (*startup_command_line != L'"')
            {
                // keep scanning until closing quote
                ++startup_command_line;
            }
            else
            {
                // found closing quote, start compaction
                quote_write_ptr = startup_command_line;
                ++startup_command_line;
                state = 3;
            }
            break;

        case 3: // just after a closing quote
        {
            wchar_t ch = *startup_command_line;

            switch (ch)
            {
            case L'\0':
                // end of string, terminate at the old quote location
                *quote_write_ptr = L'\0';
                g_kablam_shell_arguments_count = argument_count;
                return;

            case L'\t':
            case L' ':
                // argument ends here; terminate at quote, go back to scanning
                ++startup_command_line;
                *quote_write_ptr = L'\0';
                state = 0;
                break;

            case L'"':
                // another quote: escaped quote / continuation
                ++startup_command_line;
                // fallthrough into copy state

            default:
                // start copying/compacting following characters
                ++quote_write_ptr;
                state = 4;
                break;
            }
            break;
        }

        case 4: // copying/compacting characters after a quote
        {
            wchar_t ch = *startup_command_line;

            if (ch == L'\0')
            {
                *quote_write_ptr = L'\0';
                g_kablam_shell_arguments_count = argument_count;
                return;
            }

            if (ch == L'"')
            {
                // another quote encountered; go back to "just after quote" logic
                ++startup_command_line;
                state = 3;
            }
            else
            {
                *quote_write_ptr++ = ch;
                ++startup_command_line;
            }
            break;
        }

        default:
            // safety; shouldn't be reachable
            g_kablam_shell_arguments_count = argument_count;
            return;
        }
    }
}
