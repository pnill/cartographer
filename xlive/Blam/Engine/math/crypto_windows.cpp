#include "stdafx.h"
#include "crypto_windows.h"

#include "networking/network_event.h"

/* constants */

static const wchar_t k_crypto_provider[] = L"Microsoft Enhanced RSA and AES Cryptographic Provider";

/* public code */

bool crypto_windows_sha256_hash_data(
	void const *data,
	uint32 size,
	s_sha256_hash* hash)
{
	HCRYPTPROV provider = NULL;
	HCRYPTHASH hash_handle = NULL;
	uint32 hash_length = k_sha256_hash_size_bytes;
	bool success = false;

	if (!CryptAcquireContextW(&provider, NULL, k_crypto_provider, PROV_RSA_AES, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
	{
		error(_error_delayed, "CryptAcquireContext failed: 0x%x", GetLastError());
	}
	else if (!CryptCreateHash(provider, CALG_SHA_256, 0, 0, &hash_handle))
	{
		error(_error_delayed, "CryptCreateHash failed: 0x%x", GetLastError());
	}
	else if (!CryptHashData(hash_handle, (uint8 const*)data, size, 0))
	{
		error(_error_delayed, "CryptHashData failed: 0x%x", GetLastError());
	}
	else if (!CryptGetHashParam(hash_handle, HP_HASHVAL, hash->data, &hash_length, 0))
	{
		error(_error_delayed, "CryptGetHashParam failed: 0x%x", GetLastError());
	}
	else
	{
		success = true;

		ASSERT(hash_length == sizeof(*hash));

		// Originally logged with error but changed to verbose event so we don't have it spammed constantly
		//error(_error_delayed, "Successfully hashed %d bytes", size);

		event(_event_verbose, "Successfully hashed %d bytes", size);

	}
	
	if (hash_handle)
	{
		CryptDestroyHash(hash_handle);
		hash_handle = NULL;
	}
	
	if (provider)
	{
		CryptReleaseContext(provider, 0);
	}
	
	return success;
}

bool crypto_windows_random_bytes(
	void* buffer,
	uint32 size)
{
	HCRYPTPROV provider = NULL;
	bool success = false;

	if (!CryptAcquireContextW(&provider, NULL, k_crypto_provider, PROV_RSA_AES, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
	{
		error(_error_delayed, "CryptAcquireContext (random) failed: 0x%x", GetLastError());
	}
	else if (!CryptGenRandom(provider, size, (BYTE*)buffer))
	{
		error(_error_delayed, "CryptGenRandom failed: 0x%x", GetLastError());
	}
	else
	{
		success = true;
	}

	if (provider)
	{
		CryptReleaseContext(provider, 0);
	}

	return success;
}