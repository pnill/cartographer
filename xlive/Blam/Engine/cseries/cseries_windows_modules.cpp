#include "stdafx.h"
#include "cseries_windows_modules.h"

#include "cseries_strings.h"

s_loaded_module_info g_module_info[k_recorded_module_count];


s_loaded_module_info* get_loaded_module_info()
{
    return g_module_info;
}

uint32 module_info_populate(uint32 process_id, s_loaded_module_info* module_info)
{
    HMODULE temp_module_handle[k_recorded_module_count];
    uint32 module_count = 0;

    // Get a handle to the process.
    HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
    if (process_handle != NULL)
    {
        DWORD total_size;
        // Get a list of all the modules in this process.
        if (EnumProcessModulesEx(process_handle, temp_module_handle, sizeof(temp_module_handle), &total_size, LIST_MODULES_DEFAULT))
        {
            module_count = total_size / sizeof(HMODULE);

            // Loop through every module and allocate the memory for their name and list the handles
            for (uint32 i = 0; i < module_count; i++)
            {
                // Get the full path to the module's file.
                if (GetModuleFileNameExW(process_handle, temp_module_handle[i], module_info[i].module_path.get_buffer(), MAX_PATH) == false)
                {
                    LOG_ERROR_FUNCW(L"GetModuleFileNameEx returned error: {}", GetLastError());
                }
                module_info[i].module_handle = temp_module_handle[i];
            }
        }

        // Release the handle to the process.
        CloseHandle(process_handle);
    }

    return module_count;
}

int compare_loaded_module_info(const void* a, const void* b)
{
    s_loaded_module_info* new_a = (s_loaded_module_info*)a;
    s_loaded_module_info* new_b = (s_loaded_module_info*)b;
    return (new_a->module_handle - new_b->module_handle);
}

void* module_data_load(const wchar_t* file_name)
{
    HANDLE file = CreateFileW(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        LOG_ERROR_FUNCW(L"Could not read file: %S", file_name);
    }

    DWORD bytes_read = 0;
    size_t file_size = GetFileSize(file, NULL);
    void* file_data = malloc(file_size);

    // Read the file data into file_data
    if (ReadFile(file, file_data, file_size, &bytes_read, NULL) == false)
    {
        DWORD error = GetLastError();
        if (error != 0)
        {
            LOG_ERROR_FUNCW(L"ReadFile returned error: {}", error);
        }
    }

    return file_data;
}

void module_data_free(void* file_data)
{
    free(file_data);
    return;
}

uintptr_t get_module_base_address(uint32 process_id, const wchar_t* module_name)
{
    //initialize to zero for error checking
    uintptr_t modBaseAddr = 0;

    //get a handle to a snapshot of all modules
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);

    //check if it's valid
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        //this struct holds the actual module information
        MODULEENTRY32 modEntry;

        //this is required for the function to work
        modEntry.dwSize = sizeof(modEntry);

        //If a module exists, get it's entry
        if (Module32First(hSnap, &modEntry))
        {
            //loop through the modules
            do
            {
                //compare the module name against ours
                if (!_wcsicmp(modEntry.szModule, module_name))
                {
                    //copy the base address and break out of the loop
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }

                //each iteration we grab the next module entry
            } while (Module32Next(hSnap, &modEntry));
        }
    }

    //free the handle
    CloseHandle(hSnap);
    return modBaseAddr;
}

uint32 get_crashed_module_index(const s_loaded_module_info* module_info, uint32 module_count, uint32 crash_address)
{
    size_t crashed_module_index = 0;
    // Get the module we crashed in
    for (size_t i = 0; i < module_count; i++)
    {
        if ((DWORD)module_info[i].module_handle <= crash_address)
        {
            crashed_module_index = i;
        }
        else
        {
            break;
        }
    }

    return crashed_module_index;
}