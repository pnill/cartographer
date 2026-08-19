#include "stdafx.h"
#include "kablam_strings.h"

/* constants */

enum
{
	k_default_system_language = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	k_english_language = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
	k_spanish_language = MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH),
};

/* public code */

kablam_string::kablam_string(void)
{
	m_string_handle = nullptr;
	m_string = nullptr;
	m_string_found = false;
	m_string_is_resource_handle = false;
	return;
}

kablam_string::kablam_string(
	int32 string_id)
{
	m_string_handle = nullptr;
	m_string = nullptr;
	m_string_found = false;
	m_string_is_resource_handle = false;

	load(string_id);
	return;
}

errno_t kablam_string::load(
	int32 string_id)
{
	free();

	short* string_resource = load_resource(string_id, k_default_system_language);
	size_t string_size = 0;

	if (!string_resource || (string_size = *string_resource) == 0)
	{
		string_resource = load_resource(string_id, k_english_language);
		if (!string_resource || (string_size = *string_resource) == 0)
		{
			assert(false && "error not loading string");
		}
	}
	wchar_t* string = new wchar_t[2 * string_size + 1];
	m_string_handle = (HANDLE)string;
	if (!string)
	{
		assert(false && "could not allocate memory for string");
	}

	m_string_found = true;
	m_string_is_resource_handle = false;
	return wcsncpy_s(string, string_size + 1, (wchar_t*)(string_resource + 1), string_size);
}

wchar_t const* kablam_string::get(void) const
{
	wchar_t const* result = m_string;

	if (m_string_found)
	{
		result = (wchar_t*)m_string_handle;
	}

	return result;
}

void kablam_string::free(void)
{
	if (m_string_handle && m_string_found)
	{
		if (m_string_is_resource_handle)
		{
			LocalFree(m_string_handle);
		}
		else
		{
			::free(m_string_handle);
		}

		m_string_found = false;
		m_string_is_resource_handle = false;
		m_string_handle = nullptr;
	}

	m_string = nullptr;
	return;
}

short* kablam_string::load_resource(
	int32 string_id,
	WORD language)
{
	HMODULE main_module = GetModuleHandleW(NULL);

	HRSRC resource_handle = FindResourceExW(main_module, RT_STRING, MAKEINTRESOURCEW((string_id >> 4) + 1), language);

	if (!resource_handle)
	{
		return nullptr;
	}
	
	HGLOBAL string_handle = LoadResource(main_module, resource_handle);

	if (!string_handle)
	{
		return nullptr;
	}

	short* string = (short*)LockResource(string_handle);

	if (string)
	{
		int32 index = string_id & 15;

		for (int32 i = 0; i < index; ++i)
		{
			WORD len = (WORD)*string;
			string += len + 1;
		}
	}

	FreeResource(string_handle);
	return string;
}
