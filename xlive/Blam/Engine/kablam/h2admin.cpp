#include "stdafx.h"

#include "kablam_command/kablam_command.h"
#include "kablam_command/kablam_command_table.h"
#include "kablam_rpc/kablam_rpc.h"
#include "kablam_rpc/kablam_rpc_client.h"

#include "kablam_shell.h"
#include "kablam_strings.h"

/* prototypes */

static bool valid_instance_name(wchar_t const* instance_name);

static void print_admin_usage_and_die(void);

/* public code */

int main(
    int argc,
    char const** argv,
    char const** envp)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);
    UNREFERENCED_PARAMETER(envp);

    kablam_shell_initialize();
    kablam_rpc_client_initialize();

    wchar_t* instance_name = nullptr;
    wchar_t instance_name_buffer[12];

    if (kablam_shell_argument_exists(L"-live"))
    {
        g_instance_is_live = true;

        instance_name = kablam_shell_argument_get_value(L"-instance:");

        if (!valid_instance_name(instance_name) || !_wcsicmp(instance_name, L"LAN"))
        {
            print_admin_usage_and_die();
        }

        wcsncpy_s(instance_name_buffer, NUMBEROF(instance_name_buffer), instance_name, _TRUNCATE);
        _wcsupr_s(instance_name_buffer, NUMBEROF(instance_name_buffer));
        instance_name = instance_name_buffer;
    }
    else if (kablam_shell_argument_exists(L"-lan"))
    {
        g_instance_is_lan = true;

        wcsncpy_s(instance_name_buffer, NUMBEROF(instance_name_buffer), L"LAN", _TRUNCATE);
        instance_name = instance_name_buffer;
    }
    else
    {
        print_admin_usage_and_die();
    }

    kablam_string copyright;
    kablam_string title;

    copyright.load(kablam_string_copyright_2006_2007_ms);
    title.load(kablam_string_title_admin_console);

    wprintf(L"%ls [%ls %ls %d]\r\n%ls\r\n", title.get(), L"", L"11122.07.08.24.1808.main", 11122, copyright.get());

    copyright.free();
    title.free();

    wchar_t instance_username[256]{};

    // h2admin originally would grab the username associated with the service from here to use in the RPC connection
    // however this seems to be unnecessary to do to actually connect to the RPC service, potentially a security feature they
    // did not fully implement into H2Admin.
    // if the username wasn't found h2admin would just close

    // bool instance_service_username_found = kablam_registry_get_instance_service_username(instance_name, &instance_username, 256);

    bool instance_service_username_found = true;

    //
    // if(!instance_service_username_found)
    // {
    //      print_admin_usage_and_die();
    // }
    //
    // wprintf(L"LRPC Service username: %s", instance_username);

    if (!instance_service_username_found)
    {
        kablam_string_quick_wprintf(L"%s\r\n", kablam_string_err_instance_not_found);

        return 2;
    }

    kablam_string_quick_wprintf(L"\r\n%s", kablam_string_info_connecting_to_server);

    if (instance_name)
        wprintf(L" (%s)", instance_name);

    wprintf(L"...\r\n");

    fflush(stdout);

    kablam_rpc_client* rpc_client = kablam_rpc_client_get();
    kablam_rpc_service_version_info rpc_version_info{ };

    if (!rpc_client->connect_local(instance_name, instance_username, &rpc_version_info))
    {
        kablam_string_quick_wprintf(L"%s\r\n", kablam_string_err_connect_failed);

        rpc_client->disconnect_local();

        return 1;
    }

    wprintf(L"LRPC Endpoint: %s\r\n", rpc_client->m_rpc_endpoint);

    kablam_string_quick_wprintf(L"%s\r\n\r\n", kablam_string_info_connected);

    copyright.load(kablam_string_copyright_2006_ms);
    title.load(kablam_string_title_h2_dedicated_server);

    wprintf(L"%s [%S %S %d]\r\n%s\r\n", title.get(), rpc_version_info.unknown_version, rpc_version_info.build_string, rpc_version_info.build_version, copyright.get());

    copyright.free();
    title.free();

    bool do_main_loop = true;

    wchar_t stdin_buffer[1024];
    while (do_main_loop)
    {
        if (instance_name)
            wprintf(L"\r\nh2server #%s$ ", instance_name);
        else
            wprintf(L"\r\nh2server$ ");

        if (kablam_shell_read_input(stdin_buffer, NUMBEROF(stdin_buffer)))
        {
            uint32 command_argument_count = 0;
            wchar_t* command_arguments[512] = {};

            kablam_command_table_parse_input(stdin_buffer, command_arguments, 128, &command_argument_count);

            if (command_argument_count)
            {
                uint32 command_table_index = 0;

                kablam_command* command = kablam_command_table_create_command(command_arguments, command_argument_count, true, &command_table_index);

                if (command)
                {
                    if (command->valid())
                    {
                        RpcTryExcept
                        {
	                        command->execute_rpc_command();

	                        kablam_command_table_parse_response(command, command_table_index);
                        }
                        RpcExcept(1)
                        {
                            uint32 ulCode = RpcExceptionCode();
                            printf("Runtime reported exception: %lu\n", ulCode);

                            LPSTR messageBuffer = nullptr;
                            size_t size = FormatMessageA(
                                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL,
                                ulCode,
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                (LPSTR)&messageBuffer,
                                0,
                                NULL);

                            if (size > 0 && messageBuffer != nullptr) 
                            {
                                printf("Detailed message: %s\r\n", messageBuffer);
                                LocalFree(messageBuffer);
                            }
                            else 
                            {
                                printf("Could not retrieve detailed message for error code.");
                            }
                        }
                        RpcEndExcept
                    }
                    else
                    {
                        kablam_command_table_print_help_function(command->type());
                    }

                    free(command);
                }

                wprintf(L"\r\n");
            }
        }
        else
        {
            if (kablam_shell_read_input_failed())
            {
                exit(-1);
            }
            else
            {
                kablam_string_quick_wprintf(L"%s\r\n", kablam_string_err_command_too_long);
            }
            do_main_loop = false;
            break;
        }
    }

    rpc_client->disconnect_local();

    kablam_rpc_client_free();
    return 0;
}

// Memory allocation function for RPC.
void* __RPC_USER midl_user_allocate(
    size_t size)
{
    // todo: move to cseries allocation?
    return malloc(size);
}

// Memory deallocation function for RPC.
void __RPC_USER midl_user_free(
    void* p)
{
    // todo: move to cseries free?
    free(p);
    return;
}

/* private code */

static bool valid_instance_name(
    wchar_t const* instance_name)
{
    assert(instance_name != NULL);

    int length = 0;

    for (wchar_t const * p = instance_name; *p != L'\0'; ++p)
    {
        wchar_t ch = *p;

        bool is_upper = (ch >= L'A' && ch <= L'Z');
        bool is_lower = (ch >= L'a' && ch <= L'z');
        bool is_digit = (ch >= L'0' && ch <= L'9');

        if (!is_upper && !is_lower && !is_digit)
            return false;

        ++length;
        if (length >= 13)
            return false;
    }

    return (length > 0);
}

static void print_admin_usage_and_die(void)
{
    kablam_string_quick_wprintf(L"%s\r\n\r\n", kablam_string_help_admin_cli_usage);
    exit(1);
    return;
}
