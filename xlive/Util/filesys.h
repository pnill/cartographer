#pragma once

// Sets global exe_directory using GetModuleFileNameW
void set_exe_directory();

// Returns pointer to exe_directory
wchar_t* get_exe_directory();

// TODO We should not be using this in the future incase people are using non latin characters in their user profile
// only included because I dont want to touch everything that uses this right now
char* get_exe_directory_narrow();
