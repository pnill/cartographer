#pragma once

LONG WINAPI debug_unhandled_exception_cb(_In_ struct _EXCEPTION_POINTERS* ExceptionInfo);

// Get nt headers from pe_module
PIMAGE_NT_HEADERS get_nt_headers(void* pe_module);

uintptr_t devirtualize_address(void* pe_module, uintptr_t virtual_address, DWORD base_address);

void cseries_debug_initialize();
