#include "stdafx.h"
#include "bitstream.h"
#include "H2MOD.h"

void bitstream::data_encode_string(char* name, int a3, int a4)
{
	typedef void(__thiscall* data_encode_string_def)(bitstream* thisx, char* name, int a3, int a4);
	auto p_data_encode_string = h2mod->GetAddress<data_encode_string_def>(0xD18DF, 0xCDE99);
	p_data_encode_string(this, name, a3, a4);
}

void bitstream::data_decode_string(char* name, int a3, int a4)
{
	typedef void(__thiscall* data_decode_string_def)(bitstream* thisx, char* name, int a3, int a4);
	auto p_data_decode_string = h2mod->GetAddress<data_decode_string_def>(0xD1FA7, 0xCE561);
	p_data_decode_string(this, name, a3, a4);
}

void bitstream::data_encode_integer(char* name, unsigned int value, signed int a3)
{
	typedef void(__thiscall* data_encode_integer_def)(bitstream* thisx, char* name, unsigned int value, signed int a3);
	auto p_data_encode_integer = h2mod->GetAddress<data_encode_integer_def>(0xD17C6, 0xCDD80);
	p_data_encode_integer(this, name, value, a3);
}

int bitstream::data_decode_integer(char* name, int a3)
{
	typedef int(__thiscall* data_decode_integer_def)(bitstream* thisx, char* name, int a3);
	auto p_data_decode_integer = h2mod->GetAddress<data_decode_integer_def>(0xD1EE5, 0xCE49F);
	return p_data_decode_integer(this, name, a3);
}

void bitstream::data_encode_bits(char* name, void* a3, int a4)
{
	typedef void(__thiscall* data_encode_bits_def)(bitstream* thisx, char* name, void* a3, int a4);
	auto p_data_encode_bits = h2mod->GetAddress<data_encode_bits_def>(0xD18CD, 0xCDE87);
	p_data_encode_bits(this, name, a3, a4);
}

void bitstream::data_decode_bits(char* name, int a3, int a4)
{
	typedef void(__thiscall* data_decode_bits_def)(bitstream* thisx, char* name, int a3, int a4);
	auto p_data_decode_bits = h2mod->GetAddress<data_decode_bits_def>(0xD1F95, 0xCE54F);
	p_data_decode_bits(this, name, a3, a4);
}

void bitstream::data_encode_bool(char* name, bool a3)
{
	typedef void(__thiscall* data_encode_bool_def)(bitstream* thisx, char* name, bool a3);
	auto p_data_encode_bool = h2mod->GetAddress<data_encode_bool_def>(0xD1886, 0xCDE40);
	p_data_encode_bool(this, name, a3);
}

bool bitstream::data_decode_bool(char* name)
{
	typedef bool(__thiscall* data_decode_bool_def)(bitstream* thisx, char* name);
	auto p_data_decode_bool = h2mod->GetAddress<data_decode_bool_def>(0xD1F47, 0xCE501);
	return p_data_decode_bool(this, name);
}

void bitstream::data_decode_address(char* name, int a3)
{
	typedef void(__thiscall* data_decode_address_def)(bitstream* thisx, char* name, int a3);
	auto p_data_decode_address = h2mod->GetAddress<data_decode_address_def>(0xD1FFD, 0xCE5B7);
	return p_data_decode_address(this, name, a3);
}

bool bitstream::packet_is_valid()
{
	typedef bool(__thiscall* packet_is_valid_def)(bitstream* thisx);
	auto p_packet_is_valid = h2mod->GetAddress<packet_is_valid_def>(0xD114C, 0xCD706);
	return p_packet_is_valid(this);
}
