#include "stdafx.h"
#include "curl-interface.h"

#include <windows.h>

static unsigned char* g_cacert_data = nullptr;
static long g_cacert_len = 0;

extern "C" int wolfSSL_CTX_load_verify_buffer(void* ctx, const unsigned char* in, long sz, int format);

static bool cartographer_load_ca_bundle(const char* path)
{
	if (g_cacert_data != nullptr)
	{
		return true;
	}

	HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return false;

	DWORD size = GetFileSize(file, NULL);
	if (size == INVALID_FILE_SIZE || size == 0)
	{
		CloseHandle(file);
		return false;
	}

	unsigned char* buffer = static_cast<unsigned char*>(malloc(size));
	if (buffer == nullptr)
	{
		CloseHandle(file);
		return false;
	}

	DWORD bytes_read = 0;
	BOOL read_ok = ReadFile(file, buffer, size, &bytes_read, NULL);
	CloseHandle(file);

	if (!read_ok || bytes_read != size)
	{
		free(buffer);
		return false;
	}

	g_cacert_data = buffer;
	g_cacert_len = static_cast<long>(size);
	return true;
}

static CURLcode cartographer_ssl_ctx_callback(CURL* curl, void* ssl_ctx, void* userptr)
{
	(void)curl;
	(void)userptr;

	if (ssl_ctx == nullptr || g_cacert_data == nullptr || g_cacert_len <= 0)
		return CURLE_SSL_CACERT_BADFILE;

	const int loaded = wolfSSL_CTX_load_verify_buffer(ssl_ctx, g_cacert_data, g_cacert_len, 1);
	if (loaded != 1)
		return CURLE_SSL_CACERT_BADFILE;

	return CURLE_OK;
}

CURL* curl_interface_init()
{
	return curl_easy_init();
}

CURL* curl_interface_init_no_verify()
{
	CURL* curl = curl_interface_init();
	if (curl != nullptr)
	{

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Cartographer " DLL_VERSION_STR);

		return curl;
	}

	return nullptr;
}

CURL* curl_interface_init_verified()
{
	CURL* curl = curl_interface_init();
	if (curl == nullptr)
	{
		return nullptr;
	}

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Cartographer " DLL_VERSION_STR);

	char ca_bundle_path[MAX_PATH] = { 0 };
	HMODULE module_handle = nullptr;
	if (GetModuleHandleExA(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCSTR>(&curl_interface_init_verified),
		&module_handle)
		&& module_handle != nullptr
		&& GetModuleFileNameA(module_handle, ca_bundle_path, ARRAYSIZE(ca_bundle_path)) > 0)
	{
		char* last_separator = strrchr(ca_bundle_path, '\\');
		if (last_separator != nullptr)
		{
			strcpy_s(last_separator + 1, ARRAYSIZE(ca_bundle_path) - (last_separator + 1 - ca_bundle_path), "cacert.pem");
			curl_easy_setopt(curl, CURLOPT_CAINFO, ca_bundle_path);
		}
	}

	if (ca_bundle_path[0] != '\0' && cartographer_load_ca_bundle(ca_bundle_path))
	{
		curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, cartographer_ssl_ctx_callback);
		curl_easy_setopt(curl, CURLOPT_SSL_CTX_DATA, nullptr);
	}

	return curl;
}
