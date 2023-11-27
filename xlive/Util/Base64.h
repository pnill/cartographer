#pragma once

/*
	https://github.com/ReneNyffenegger/cpp-base64
*/

#include <string>

std::string base64_encode(unsigned char const*, unsigned int len);
std::string base64_decode(std::string const& s);
std::string base64_encode_binary(const std::vector<unsigned char>& binaryData);
std::vector<unsigned char> base64_decode_binary(const std::string& encoded_string);