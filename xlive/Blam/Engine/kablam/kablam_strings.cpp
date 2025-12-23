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

kablam_string::kablam_string()
{
	this->m_string_handle = nullptr;
	this->m_string = nullptr;
	this->m_string_found = false;
	this->m_string_is_resource_handle = false;
}

kablam_string::kablam_string(int32 string_id)
{
	this->m_string_handle = nullptr;
	this->m_string = nullptr;
	this->m_string_found = false;
	this->m_string_is_resource_handle = false;

	this->load(string_id);
}

errno_t kablam_string::load(int32 string_id)
{
	this->free();

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
	this->m_string_handle = (HANDLE)string;
	if (!string)
	{
		assert(false && "could not allocate memory for string");
	}

	this->m_string_found = true;
	this->m_string_is_resource_handle = false;
	return wcsncpy_s(string, string_size + 1, (wchar_t*)(string_resource + 1), string_size);
}

const wchar_t* kablam_string::get() const
{
	if (!this->m_string_found)
		return this->m_string;

	return (wchar_t*)this->m_string_handle;
}

void kablam_string::free()
{
	if (this->m_string_handle && this->m_string_found)
	{
		if (this->m_string_is_resource_handle)
			LocalFree(this->m_string_handle);
		else
			::free(this->m_string_handle);

		this->m_string_found = false;
		this->m_string_is_resource_handle = false;
		this->m_string_handle = nullptr;
	}

	this->m_string = nullptr;
}

short* kablam_string::load_resource(int32 string_id, WORD language)
{
	HMODULE main_module = GetModuleHandleW(NULL);

	HRSRC resource_handle = FindResourceExW(main_module, RT_STRING, MAKEINTRESOURCEW((string_id >> 4) + 1), language);

	if (!resource_handle)
		return nullptr;

	HGLOBAL string_handle = LoadResource(main_module, resource_handle);

	if (!string_handle)
		return nullptr;

	short* string = (short*)LockResource(string_handle);

	if (string)
	{
		int index = string_id & 15;

		for (int i = 0; i < index; ++i)
		{
			WORD len = (WORD)*string;
			string += len + 1;
		}
	}

	FreeResource(string_handle);
	return string;
}
