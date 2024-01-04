#include "stdafx.h"

#include "bitstream.h"

void bitstream::set_data(uint8* stream_buf, int32 stream_buf_size)
{
	m_stream_buf = stream_buf;
	m_stream_buf_size_bytes = stream_buf_size;
	reset(_bitstream_state_none);
}

void bitstream::begin_reading()
{
	if (!writing())
	{
		bitstream::reset(_bitstream_state_reading);
	}

	int32 header = data_decode_integer("bitstream-header", 32);
	if (header == 'debg')
	{
		// comment this line if you want to read debug streams
		// but probably isn't recommended
		m_error_stream_debug_mode_is_enabled = true;
	}
	else
	{
		m_current_bit_position = 0;
		m_error_stream_debug_mode_is_enabled = false;
	}
}

void bitstream::finish_reading()
{
	m_state = _bitstream_state_reading_finished;
}

void bitstream::begin_writing(int32 alignment)
{	
	typedef void(__thiscall* begin_writing_t)(bitstream*, uint32, int, int);
	auto p_begin_writing = Memory::GetAddress<begin_writing_t>(0xD24F5, 0xCEAAF);
	p_begin_writing(this, alignment, 0, 0);	// last 2 arguments are debug
}

void bitstream::finish_writing(int32* out_space_left_in_bits)
{
	typedef void(__thiscall* finish_writing_t)(bitstream*, void*);
	auto p_finish_writing = Memory::GetAddress<finish_writing_t>(0xD1205, 0xCD7BF);
	p_finish_writing(this, out_space_left_in_bits);
}

void bitstream::reset(e_bitstream_state state)
{
	m_state = state;
	m_current_bit_position = 0;
	m_position_stack_depth = 0;
	m_error_stream_debug_mode_is_enabled = false;
	if (writing())
	{
		// enable some debugging, not really required in release?
	}
	else if (reading())
	{
		// enable some debugging, not really required in release?
	}
}

void bitstream::data_encode_string_wide(const char* name, void* string, int size_in_words)
{
	typedef void(__thiscall* data_encode_string_t)(bitstream*, const char*, void*, int);
	auto p_data_encode_string = Memory::GetAddress<data_encode_string_t>(0xD18DF, 0xCDE99);
	p_data_encode_string(this, name, string, size_in_words);
}

void bitstream::data_decode_string_wide(const char* name, void* string_buffer, int size_in_words)
{
	typedef void(__thiscall* data_decode_string_t)(bitstream*, const char*, void*, int);
	auto p_data_decode_string = Memory::GetAddress<data_decode_string_t>(0xD1FA7, 0xCE561);
	p_data_decode_string(this, name, string_buffer, size_in_words);
}

void bitstream::data_encode_integer(const char* name, unsigned int value, unsigned int size_in_bits)
{
	typedef void(__thiscall* data_encode_integer_t)(bitstream*, const char*, unsigned int, int);
	auto p_data_encode_integer = Memory::GetAddress<data_encode_integer_t>(0xD17C6, 0xCDD80);
	p_data_encode_integer(this, name, value, size_in_bits);
}

int bitstream::data_decode_integer(const char* name, unsigned int size_in_bits)
{
	typedef int(__thiscall* data_decode_integer_t)(bitstream*, const char*, int);
	auto p_data_decode_integer = Memory::GetAddress<data_decode_integer_t>(0xD1EE5, 0xCE49F);
	return p_data_decode_integer(this, name, size_in_bits);
}

void bitstream::data_encode_bits(const char* name, void* data, unsigned int size_in_bits)
{
	typedef void(__thiscall* data_encode_bits_t)(bitstream*, const char*, void*, int);
	auto p_data_encode_bits = Memory::GetAddress<data_encode_bits_t>(0xD18CD, 0xCDE87);
	p_data_encode_bits(this, name, data, size_in_bits);
}

void bitstream::data_decode_bits(const char* name, void* data, int size_in_bits)
{
	typedef void(__thiscall* data_decode_bits_t)(bitstream*, const char*, void*, int);
	auto p_data_decode_bits = Memory::GetAddress<data_decode_bits_t>(0xD1F95, 0xCE54F);
	p_data_decode_bits(this, name, data, size_in_bits);
}

void bitstream::data_encode_bool(const char* name, bool value)
{
	typedef void(__thiscall* data_encode_bool_t)(bitstream*, const char*, bool);
	auto p_data_encode_bool = Memory::GetAddress<data_encode_bool_t>(0xD1886, 0xCDE40);
	p_data_encode_bool(this, name, value);
}

bool bitstream::data_decode_bool(const char* name)
{
	typedef bool(__thiscall* data_decode_bool_t)(bitstream*, const char*);
	auto p_data_decode_bool = Memory::GetAddress<data_decode_bool_t>(0xD1F47, 0xCE501);
	return p_data_decode_bool(this, name);
}

void bitstream::data_decode_address(const char* name, void* address)
{
	typedef void(__thiscall* data_decode_address_t)(bitstream*, const char*, void*);
	auto p_data_decode_address = Memory::GetAddress<data_decode_address_t>(0xD1FFD, 0xCE5B7);
	return p_data_decode_address(this, name, address);
}

void bitstream::data_encode_quantized_real(const char* name, float value, float min_value, float max_value, int size_in_bits, char exact_midpoint)
{
	typedef void(__thiscall* data_encode_quantized_real_t)(bitstream*, const char*, float, float, float, int, char);
	auto p_data_encode_quantized_real = Memory::GetAddress<data_encode_quantized_real_t>(0xD1B9B, 0xCE155);
	p_data_encode_quantized_real(this, name, value, min_value, max_value, size_in_bits, exact_midpoint);
}

float bitstream::data_decode_quantized_real(const char* name, float min_value, float max_value, int size_in_bits, char exact_midpoint)
{
	typedef float(__thiscall* data_decode_quantized_real_t)(bitstream*, const char*, float, float, int, char);
	auto p_data_decode_quantized_real = Memory::GetAddress<data_decode_quantized_real_t>(0xD2039, 0xCE5F3);
	return p_data_decode_quantized_real(this, name, min_value, max_value, size_in_bits, exact_midpoint);
}

void bitstream::data_encode_unit_vector(const char* name, real_vector3d* vector)
{
	typedef void(__thiscall* data_encode_unit_vector_t)(bitstream*, const char*, real_vector3d*);
	auto p_data_encode_unit_vector = Memory::GetAddress<data_encode_unit_vector_t>(0xD1BD9, 0xCE193);
	p_data_encode_unit_vector(this, name, vector);
}

void bitstream::data_decode_unit_vector(const char* name, real_vector3d* out_vector)
{
	typedef void(__thiscall* data_decode_unit_vector_t)(bitstream*, const char*, real_vector3d*);
	auto p_data_decode_unit_vector = Memory::GetAddress<data_decode_unit_vector_t>(0xD20F4, 0xCE6AE);
	p_data_decode_unit_vector(this, name, out_vector);
}

void bitstream::data_encode_signed_integer(const char* name, int value, unsigned int size_in_bits)
{
	typedef void(__thiscall* data_encode_signed_integer_t)(bitstream*, const char*, int, int);
	auto p_data_encode_signed_integer = Memory::GetAddress<data_encode_signed_integer_t>(0xD183B, 0xCDDF5);
	p_data_encode_signed_integer(this, name, value, size_in_bits);
}

int bitstream::data_decode_signed_integer(const char* name, unsigned int size_in_bits)
{
	typedef int(__thiscall* data_decode_signed_integer_t)(bitstream*, const char*, unsigned int);
	auto p_data_decode_signed_integer = Memory::GetAddress<data_decode_signed_integer_t>(0xD1EF2, 0xCE4AC);
	return p_data_decode_signed_integer(this, name, size_in_bits);
}

void bitstream::data_encode_axes(const char* name, real_vector3d* forward, real_vector3d* up)
{
	typedef void(__thiscall* data_encode_axes_t)(bitstream*, const char*, real_vector3d*, real_vector3d*);
	auto p_data_encode_axes = Memory::GetAddress<data_encode_axes_t>(0xD1D41, 0xCE2FB);
	p_data_encode_axes(this, name, forward, up);
}

void bitstream::data_decode_axes(const char* name, real_vector3d *out_forward, real_vector3d *out_up)
{
	typedef void(__thiscall* data_decode_axes_t)(bitstream*, const char*, real_vector3d*, real_vector3d*);
	auto p_data_decode_axes = Memory::GetAddress<data_decode_axes_t>(0xD21DB, 0xCE795);
	p_data_decode_axes(this, name, out_forward, out_up);
}

void bitstream::data_encode_vector(const char *name, real_vector3d *vector, float min_magnitude_value, float max_magnitude_value, int magnitude_size_in_bits)
{
	typedef void(__thiscall* data_encode_vector_t)(bitstream*, const char*, real_vector3d*, float , float , int );
	auto p_data_encode_vector = Memory::GetAddress<data_encode_vector_t>(0xD1C4B, 0xCE205);
	p_data_encode_vector(this, name, vector, min_magnitude_value, max_magnitude_value, magnitude_size_in_bits);
}

void bitstream::data_decode_vector(const char *name, real_vector3d *out_vector, float min_magnitude_value, float max_magnitude_value, int magnitude_size_in_bits)
{
	typedef void(__thiscall* data_decode_vector_t)(bitstream*, const char*, real_vector3d *, float, float, int);
	auto p_data_decode_vector = Memory::GetAddress<data_decode_vector_t>(0xD210C, 0xCE6C6);
	p_data_decode_vector(this, name, out_vector, min_magnitude_value, max_magnitude_value, magnitude_size_in_bits);
}

void bitstream::data_encode_flags(const char* name, unsigned long long value, int size_in_bits)
{
	typedef void(__thiscall* data_encode_flags_t)(bitstream*, const char*, unsigned long long, int);
	auto p_data_encode_flags = Memory::GetAddress<data_encode_flags_t>(0xD171D, 0xCDCD7);
	p_data_encode_flags(this, name, value, size_in_bits);
}

unsigned long long bitstream::data_decode_flags(const char* name, int size_in_bits)
{
	typedef unsigned long long(__thiscall* data_decode_flags_t)(bitstream*, const char*, int);
	auto p_data_decode_flags = Memory::GetAddress<data_decode_flags_t>(0xD1E9A, 0xCE454);
	return p_data_decode_flags(this, name, size_in_bits);
}