#include <stdafx.h>
#include <Wincrypt.h>
#include <sstream>
#include <iomanip>
#include "hash.h"

bool hash_open_file(const char *file_name, HANDLE &file)
{
	file = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	return LOG_CHECK(file != INVALID_HANDLE_VALUE);
}

bool hash_get_crypto_provider(DWORD type, HCRYPTPROV *provider)
{
	return LOG_CHECK(CryptAcquireContext(provider,
		NULL,
		NULL,
		type,
		CRYPT_VERIFYCONTEXT));
}

#define file_chunk_size 1024

bool hash_do_file_hashing(HANDLE file, HCRYPTHASH hash, DWORD flags)
{
	BYTE file_chunk[file_chunk_size];
	DWORD bytes_read = 0;
	while (LOG_CHECK(ReadFile(file, file_chunk, file_chunk_size,
		&bytes_read, NULL)))
	{
		if (bytes_read == 0)
		{
			return true;
		}

		if (!LOG_CHECK(CryptHashData(hash, file_chunk, bytes_read, flags)))
		{
			return false;
		}
	}
	return true;
}

std::string hashes::as_hex_string(BYTE *data, size_t data_len)
{
	std::stringstream ss;
	ss << std::setfill('0') << std::hex;
	for (size_t i = 0; i < data_len; i++)
	{
		ss << std::setw(2) << static_cast<int>(data[i]);
	}
	return ss.str();
}

#define md5_len 16

bool hashes::calc_file_md5(const std::string &filename, BYTE *checksum, size_t &checksum_len)
{
	HANDLE file;
	if (hash_open_file(filename.c_str(), file))
	{
		HCRYPTPROV crypto_provider = NULL;
		if (hash_get_crypto_provider(PROV_RSA_FULL, &crypto_provider))
		{
			HCRYPTHASH hash = NULL;
			if (LOG_CHECK(CryptCreateHash(crypto_provider, CALG_MD5, 0, 0, &hash)))
			{
				if (hash_do_file_hashing(file, hash, 0))
				{
					DWORD hash_len = min(checksum_len, md5_len);
					if (LOG_CHECK(CryptGetHashParam(hash, HP_HASHVAL, checksum, &hash_len, 0)))
					{
						checksum_len = hash_len;
						CryptDestroyHash(hash);
						CryptReleaseContext(crypto_provider, 0);
						CloseHandle(file);
						return true;
					}
					checksum_len = 0;
				}
				CryptDestroyHash(hash);
			}
			CryptReleaseContext(crypto_provider, 0);
		}
		CloseHandle(file);
	}
	TRACE_FUNC_N("Failed to calculate md5 hash for %s", filename.c_str());
	return false;
}


bool hashes::calc_file_md5(const std::string &filename, std::string &checksum_out)
{
	BYTE hash_data[md5_len];
	size_t hash_len = ARRAYSIZE(hash_data);
	if (calc_file_md5(filename, hash_data, hash_len))
	{
		checksum_out = hashes::as_hex_string(hash_data, hash_len);
		return true;
	}
	return false;
}