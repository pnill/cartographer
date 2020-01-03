#pragma once

typedef void(__thiscall* data_encode_string)(void* thisx, char* a2, int a3, int a4);
typedef void(__thiscall* data_encode_integer)(void *thisx, char* name, unsigned int value, signed int a3);
typedef void(__thiscall* data_encode_bits)(void* thisx, char* a2, void* a3, int a4);
typedef	void(__thiscall* data_encode_boolean)(void* thisx, char* a2, char a3);

typedef void(__thiscall* data_decode_string)(void* thisx, char* a2, int a3, int a4);
typedef void(__thiscall* data_decode_address)(void* thisx, char* a2, int a3);
typedef int(__thiscall* data_decode_integer)(void* thisx, char* a2, int a3);
typedef void(__thiscall* data_decode_bits)(void* thisx, char* a2, int a3, int a4);
typedef bool(__thiscall* data_decode_bool)(void* thisx, char* a2);

typedef bool(__thiscall* valid_packet_type)(void* thisx);

namespace bitstream
{
	// encoding
	data_encode_string p_data_encode_string();
	data_encode_integer p_data_encode_integer();
	data_encode_bits p_data_encode_bits();
	data_encode_boolean p_data_encode_bool();

	// decoding
	data_decode_string p_data_decode_string();
	data_decode_address p_data_decode_address();
	data_decode_integer p_data_decode_integer();
	data_decode_bits p_data_decode_bits();
	data_decode_bool p_data_decode_bool();

	// verification
	valid_packet_type p_packet_is_valid();
}