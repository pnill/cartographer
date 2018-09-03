#pragma once
#include <string>

namespace hashes
{
	std::string as_hex_string(BYTE *data, size_t data_len);

	bool calc_file_md5(const std::string &filename, BYTE *checksum, size_t &checksum_len);
	bool calc_file_md5(const std::string &filename, std::string &checksum_out);
};