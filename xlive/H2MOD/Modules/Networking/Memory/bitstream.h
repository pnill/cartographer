#pragma once

#pragma pack(push, 1)
struct bitstream
{
	DWORD field_0;
	DWORD field_4;
	BYTE gap_8[8];
	DWORD field_10;

	void data_encode_string(char* name, void* string, int size_in_words);
	void data_decode_string(char* name, void* string_buffer, int size_in_words);
	void data_encode_integer(char* name, unsigned int value, unsigned int size_in_bits);
	int data_decode_integer(char* name, unsigned int size_in_bits);
	void data_encode_bits(char* name, void* data, unsigned int size_in_bits);
	void data_decode_bits(char* name, void* data, int size_in_bits);
	void data_encode_bool(char* name, bool value);
	bool data_decode_bool(char* name);
	void data_decode_address(char* name, void* address);
	bool packet_is_valid();
};
#pragma pack(pop)
