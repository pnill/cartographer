#pragma once
#include "cseries_strings.h"

// constants

#define k_recorded_module_count 256

// structures

struct s_loaded_module_info
{
    c_static_wchar_string260 module_path;
    HMODULE module_handle;
};

// declarations

// Get array of loaded modules and info
s_loaded_module_info* get_loaded_module_info();

// Gets the loaded module names and handles and stores them in the module_info parameter
// Make sure whenever you call this function to call the free_loaded_module_info function afterwards to free the memory allocated
// Returns the amount of modules currently loaded
uint32 module_info_populate(uint32 processID, s_loaded_module_info* module_info);

// Compares module handle values
int compare_loaded_module_info(const void* a, const void* b);

// Load module data from file_name passed
// Make sure whenever you call this function to call the module_data_free function afterwards to free the memory allocated
void* module_data_load(const wchar_t* file_name);

// Free module data
void module_data_free(void* file_data);

// Get base address from module
uintptr_t get_module_base_address(uint32 process_id, const wchar_t* module_name);

// Get index of crashed module
uint32 get_crashed_module_index(const s_loaded_module_info* module_info, uint32 module_count, uint32 crash_address);