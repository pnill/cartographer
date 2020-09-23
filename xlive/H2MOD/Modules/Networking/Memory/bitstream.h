#pragma once

#pragma pack(push, 1)
struct bitstream
{
	BYTE *packet_buffer;
	int packet_length_in_bytes;
	DWORD alignment;
	signed int field_C;
	signed int used_packet_length_in_bits;
	char packet_is_debug;
	BYTE gap_15[3];
	signed int field_18;
	BYTE gap_1C[16];
	DWORD field_2C;
	DWORD field_30;

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
static_assert(sizeof(bitstream) == 0x34, "invalid bitstream size");
