#include "stdafx.h"

#include "bitstream.h"

void bitstream::init(void*buffer, DWORD data_length)
{
	this->m_data = buffer;	
	this->m_data_size_bytes = data_length;
	this->state = e_bitstream_state::bitstream_state_none;

	this->m_current_bit_position = 0;
	this->m_position_stack_depth = 0;
	this->is_debug_stream = 0;
}

void bitstream::init(void*buffer, DWORD data_length, DWORD data_size_alignment)
{
	this->m_data_size_alignment = data_size_alignment;
	init(buffer, data_length);
}

void bitstream::begin_writing(DWORD data_size_alignment)
{	
	typedef void(__thiscall* begin_writing_def)(bitstream*, DWORD, char, char);
	auto p_begin_writing_def = Memory::GetAddress<begin_writing_def>(0xD24F5, 0xCEAAF);
	p_begin_writing_def(this, data_size_alignment, 0, 0);	//last 2 arguments are leftover debug stuffs
}

void bitstream::finish_writing(void* out_space_left_in_bits)
{
	typedef void(__thiscall* finish_writing_def)(bitstream*, void*);
	auto p_finish_writing_def = Memory::GetAddress<finish_writing_def>(0xD1205, 0xCD7BF);
	p_finish_writing_def(this, out_space_left_in_bits);
}

void bitstream::data_encode_string_wide(char* name, void* string, int size_in_words)
{
	typedef void(__thiscall* data_encode_string_def)(bitstream*, char*, void*, int);
	auto p_data_encode_string = Memory::GetAddress<data_encode_string_def>(0xD18DF, 0xCDE99);
	p_data_encode_string(this, name, string, size_in_words);
}

void bitstream::data_decode_string_wide(char* name, void* string_buffer, int size_in_words)
{
	typedef void(__thiscall* data_decode_string_def)(bitstream*, char*, void*, int);
	auto p_data_decode_string = Memory::GetAddress<data_decode_string_def>(0xD1FA7, 0xCE561);
	p_data_decode_string(this, name, string_buffer, size_in_words);
}

void bitstream::data_encode_integer(char* name, unsigned int value, unsigned int size_in_bits)
{
	typedef void(__thiscall* data_encode_integer_def)(bitstream*, char*, unsigned int, signed int);
	auto p_data_encode_integer = Memory::GetAddress<data_encode_integer_def>(0xD17C6, 0xCDD80);
	p_data_encode_integer(this, name, value, size_in_bits);
}

int bitstream::data_decode_integer(char* name, unsigned int size_in_bits)
{
	typedef int(__thiscall* data_decode_integer_def)(bitstream*, char*, int);
	auto p_data_decode_integer = Memory::GetAddress<data_decode_integer_def>(0xD1EE5, 0xCE49F);
	return p_data_decode_integer(this, name, size_in_bits);
}

void bitstream::data_encode_bits(char* name, void* data, unsigned int size_in_bits)
{
	typedef void(__thiscall* data_encode_bits_def)(bitstream*, char*, void*, int);
	auto p_data_encode_bits = Memory::GetAddress<data_encode_bits_def>(0xD18CD, 0xCDE87);
	p_data_encode_bits(this, name, data, size_in_bits);
}

void bitstream::data_decode_bits(char* name, void* data, int size_in_bits)
{
	typedef void(__thiscall* data_decode_bits_def)(bitstream*, char*, void*, int);
	auto p_data_decode_bits = Memory::GetAddress<data_decode_bits_def>(0xD1F95, 0xCE54F);
	p_data_decode_bits(this, name, data, size_in_bits);
}

void bitstream::data_encode_bool(char* name, bool value)
{
	typedef void(__thiscall* data_encode_bool_def)(bitstream*, char*, bool);
	auto p_data_encode_bool = Memory::GetAddress<data_encode_bool_def>(0xD1886, 0xCDE40);
	p_data_encode_bool(this, name, value);
}

bool bitstream::data_decode_bool(char* name)
{
	typedef bool(__thiscall* data_decode_bool_def)(bitstream*, char*);
	auto p_data_decode_bool = Memory::GetAddress<data_decode_bool_def>(0xD1F47, 0xCE501);
	return p_data_decode_bool(this, name);
}

void bitstream::data_decode_address(char* name, void* address)
{
	typedef void(__thiscall* data_decode_address_def)(bitstream*, char*, void*);
	auto p_data_decode_address = Memory::GetAddress<data_decode_address_def>(0xD1FFD, 0xCE5B7);
	return p_data_decode_address(this, name, address);
}

void bitstream::data_encode_quantized_real(char* name, float value, float min_value, float max_value, int size_in_bits, char exact_midpoint)
{
	typedef void(__thiscall* data_encode_quantized_real_def)(bitstream*, char*, float, float, float, int, char);
	auto p_data_encode_quantized_real_def = Memory::GetAddress<data_encode_quantized_real_def>(0xD1B9B, 0xCE155);
	p_data_encode_quantized_real_def(this, name, value, min_value, max_value, size_in_bits, exact_midpoint);
}

float bitstream::data_decode_quantized_real(char* name, float min_value, float max_value, int size_in_bits, char exact_midpoint)
{
	typedef float(__thiscall* data_decode_quantized_real_def)(bitstream*, char*, float, float, int, char);
	auto p_data_decode_quantized_real_def = Memory::GetAddress<data_decode_quantized_real_def>(0xD2039, 0xCE5F3);
	return p_data_decode_quantized_real_def(this, name, min_value, max_value, size_in_bits, exact_midpoint);
}

void bitstream::data_encode_unit_vector(char* name, real_vector3d* vector)
{
	typedef void(__thiscall* data_encode_unit_vector_def)(bitstream*, char*, real_vector3d*);
	auto p_data_encode_unit_vector_def = Memory::GetAddress<data_encode_unit_vector_def>(0xD1BD9,0xCE193);
	p_data_encode_unit_vector_def(this, name, vector);
}

void bitstream::data_decode_unit_vector(char* name, real_vector3d* out_vector)
{
	typedef void(__thiscall* data_decode_unit_vector_def)(bitstream*, char*, real_vector3d*);
	auto p_data_decode_unit_vector_def = Memory::GetAddress<data_decode_unit_vector_def>(0xD20F4,0xCE6AE);
	p_data_decode_unit_vector_def(this, name, out_vector);
}

void bitstream::data_encode_signed_integer(char* name, signed int value, unsigned int size_in_bits)
{
	typedef void(__thiscall* data_encode_signed_integer_def)(bitstream*, char*, signed int, signed int);
	auto p_data_encode_signed_integer_def = Memory::GetAddress<data_encode_signed_integer_def>(0xD183B, 0xCDDF5);
	p_data_encode_signed_integer_def(this, name, value, size_in_bits);
}

signed int bitstream::data_decode_signed_integer(char* name, unsigned int size_in_bits)
{
	typedef signed int(__thiscall* data_decode_signed_integer_def)(bitstream*, char*, unsigned int);
	auto p_data_decode_signed_integer_def = Memory::GetAddress<data_decode_signed_integer_def>(0xD1EF2, 0xCE4AC);
	return p_data_decode_signed_integer_def(this, name, size_in_bits);
}

void bitstream::data_encode_axes(char* name, real_vector3d *forward, real_vector3d *up)
{
	typedef void(__thiscall* data_encode_axes_def)(bitstream*, char*, real_vector3d*, real_vector3d*);
	auto p_data_encode_axes_def = Memory::GetAddress<data_encode_axes_def>(0xD1D41, 0xCE2FB);
	p_data_encode_axes_def(this, name, forward, up);
}

void bitstream::data_decode_axes(char* name, real_vector3d *out_forward, real_vector3d *out_up)
{
	typedef void(__thiscall* data_decode_axes_def)(bitstream*, char*, real_vector3d*, real_vector3d*);
	auto p_data_decode_axes_def = Memory::GetAddress<data_decode_axes_def>(0xD21DB, 0xCE795);
	p_data_decode_axes_def(this, name, out_forward, out_up);
}

void bitstream::data_encode_vector(char *name, real_vector3d *vector, float min_magnitude_value, float max_magnitude_value, int magnitude_size_in_bits)
{
	typedef void(__thiscall* data_encode_vector_def)(bitstream*, char*, real_vector3d *, float , float , int );
	auto p_data_encode_vector_def = Memory::GetAddress<data_encode_vector_def>(0xD1C4B, 0xCE205);
	p_data_encode_vector_def(this, name, vector, min_magnitude_value, max_magnitude_value, magnitude_size_in_bits);
}

void bitstream::data_decode_vector(char *name, real_vector3d *out_vector, float min_magnitude_value, float max_magnitude_value, int magnitude_size_in_bits)
{
	typedef void(__thiscall* data_decode_vector_def)(bitstream*, char*, real_vector3d *, float, float, int);
	auto p_data_decode_vector_def = Memory::GetAddress<data_decode_vector_def>(0xD210C, 0xCE6C6);
	p_data_decode_vector_def(this, name, out_vector, min_magnitude_value, max_magnitude_value, magnitude_size_in_bits);
}

void bitstream::data_encode_flags(char* name, unsigned long long value, int size_in_bits)
{
	typedef void(__thiscall* data_encode_flags_def)(bitstream*, char*, unsigned long long, int);
	auto p_data_encode_flags_def = Memory::GetAddress<data_encode_flags_def>(0xD171D, 0xCDCD7);
	p_data_encode_flags_def(this, name, value, size_in_bits);
}

unsigned long long bitstream::data_decode_flags(char* name, int size_in_bits)
{
	typedef unsigned long long(__thiscall* data_decode_flags_def)(bitstream*, char*, int);
	auto p_data_decode_flags_def = Memory::GetAddress<data_decode_flags_def>(0xD1E9A, 0xCE454);
	return p_data_decode_flags_def(this, name, size_in_bits);
}

bool bitstream::overflow()
{
	typedef bool(__thiscall* packet_is_valid_def)(bitstream* thisx);
	auto p_packet_is_valid = Memory::GetAddress<packet_is_valid_def>(0xD114C, 0xCD706);
	return p_packet_is_valid(this);
}

DWORD bitstream::get_space_left_in_bits()
{
	return 8 * this->m_data_size_bytes - this->m_current_bit_position;
}

DWORD bitstream::get_bytes_used()
{
	return (this->m_current_bit_position + 7) / 8;
}