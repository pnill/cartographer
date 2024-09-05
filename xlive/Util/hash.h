#pragma once

namespace hashes
{
	std::string as_hex_string(BYTE *data, size_t data_len);

	bool calc_file_md5(const std::wstring &filename, BYTE *checksum, size_t &checksum_len, long long count = 0);
	bool calc_file_md5(const std::wstring &filename, std::string &checksum_out, long long count = 0);
};