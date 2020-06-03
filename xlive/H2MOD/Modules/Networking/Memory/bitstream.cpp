#include "stdafx.h"
#include "bitstream.h"
#include "H2MOD.h"

void bitstream::data_encode_string(char* name, void* string, int size_in_words)
{
	typedef void(__thiscall* data_encode_string_def)(bitstream*, char*, void*, int);
	auto p_data_encode_string = h2mod->GetAddress<data_encode_string_def>(0xD18DF, 0xCDE99);
	p_data_encode_string(this, name, string, size_in_words);
}

void bitstream::data_decode_string(char* name, void* string_buffer, int size_in_words)
{
	typedef void(__thiscall* data_decode_string_def)(bitstream*, char*, void*, int);
	auto p_data_decode_string = h2mod->GetAddress<data_decode_string_def>(0xD1FA7, 0xCE561);
	p_data_decode_string(this, name, string_buffer, size_in_words);
}

void bitstream::data_encode_integer(char* name, unsigned int value, unsigned int size_in_bits)
{
	typedef void(__thiscall* data_encode_integer_def)(bitstream*, char*, unsigned int, signed int);
	auto p_data_encode_integer = h2mod->GetAddress<data_encode_integer_def>(0xD17C6, 0xCDD80);
	p_data_encode_integer(this, name, value, size_in_bits);
}

int bitstream::data_decode_integer(char* name, unsigned int size_in_bits)
{
	typedef int(__thiscall* data_decode_integer_def)(bitstream*, char*, int);
	auto p_data_decode_integer = h2mod->GetAddress<data_decode_integer_def>(0xD1EE5, 0xCE49F);
	return p_data_decode_integer(this, name, size_in_bits);
}

void bitstream::data_encode_bits(char* name, void* data, unsigned int size_in_bits)
{
	typedef void(__thiscall* data_encode_bits_def)(bitstream*, char*, void*, int);
	auto p_data_encode_bits = h2mod->GetAddress<data_encode_bits_def>(0xD18CD, 0xCDE87);
	p_data_encode_bits(this, name, data, size_in_bits);
}

void bitstream::data_decode_bits(char* name, void* data, int size_in_bits)
{
	typedef void(__thiscall* data_decode_bits_def)(bitstream*, char*, void*, int);
	auto p_data_decode_bits = h2mod->GetAddress<data_decode_bits_def>(0xD1F95, 0xCE54F);
	p_data_decode_bits(this, name, data, size_in_bits);
}

void bitstream::data_encode_bool(char* name, bool value)
{
	typedef void(__thiscall* data_encode_bool_def)(bitstream*, char*, bool);
	auto p_data_encode_bool = h2mod->GetAddress<data_encode_bool_def>(0xD1886, 0xCDE40);
	p_data_encode_bool(this, name, value);
}

bool bitstream::data_decode_bool(char* name)
{
	typedef bool(__thiscall* data_decode_bool_def)(bitstream*, char*);
	auto p_data_decode_bool = h2mod->GetAddress<data_decode_bool_def>(0xD1F47, 0xCE501);
	return p_data_decode_bool(this, name);
}

void bitstream::data_decode_address(char* name, void* address)
{
	typedef void(__thiscall* data_decode_address_def)(bitstream*, char*, void*);
	auto p_data_decode_address = h2mod->GetAddress<data_decode_address_def>(0xD1FFD, 0xCE5B7);
	return p_data_decode_address(this, name, address);
}

bool bitstream::packet_is_valid()
{
	typedef bool(__thiscall* packet_is_valid_def)(bitstream* thisx);
	auto p_packet_is_valid = h2mod->GetAddress<packet_is_valid_def>(0xD114C, 0xCD706);
	return p_packet_is_valid(this);
}
