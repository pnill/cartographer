#pragma once

#pragma pack(push, 1)
struct bitstream
{
	DWORD field_0;
	DWORD field_4;
	BYTE gap_8[8];
	DWORD field_10;
	
	void data_encode_string(char* name, int a3, int a4);
	void data_encode_integer(char* name, unsigned int value, signed int a3);
	void data_encode_bits(char* name, void* a3, int a4);
	void data_encode_bool(char* name, bool a3);

	void data_decode_string(char* name, int a3, int a4);
	void data_decode_address(char* name, int a3);
	int  data_decode_integer(char* name, int a3);
	void data_decode_bits(char* name, int a3, int a4);
	bool data_decode_bool(char* name);

	bool packet_is_valid();
};
#pragma pack(pop)
