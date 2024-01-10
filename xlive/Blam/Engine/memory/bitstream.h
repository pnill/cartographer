#pragma once
#include "Blam/Engine/math/real_math.h"

#define k_bitstream_default_alignment 1

enum e_bitstream_state : uint32
{
	_bitstream_state_none,
	_bitstream_state_writing,
	_bitstream_state_write_finished,
	_bitstream_state_reading,
	_bitstream_state_read_only_for_consistency,// read after write maybe???
	_bitstream_state_reading_finished,

	k_bitstream_state_count
};

struct c_bitstream
{
	uint8* m_stream_buf;
	int32 m_stream_buf_size_bytes;
	int32 m_data_size_alignment;
	e_bitstream_state m_state;
	int32 m_current_bit_position;
	bool m_error_stream_debug_mode_is_enabled; // debug streams disabled in release mode
	bool gap_15[3]; // these are other types of errors, currently disabled in release mode
	int m_position_stack_depth;
	int32 m_stack[4];
	int field_2C;
	int field_30;

	c_bitstream(uint8* stream, int32 stream_size_bytes)
	{
		// field_15 = false;
		m_data_size_alignment = k_bitstream_default_alignment;
		set_data(stream, stream_size_bytes);
	}

	~c_bitstream() = default;

	void set_data(uint8* data, int32 data_size);
	void reset(e_bitstream_state state);

	bool writing() const
	{
		return m_state == _bitstream_state_writing;
	}

	bool reading() const
	{
		return m_state == _bitstream_state_reading || m_state == _bitstream_state_read_only_for_consistency;
	}

	bool read_only_for_consistency() const
	{
		return m_state == _bitstream_state_read_only_for_consistency;
	}

	void begin_reading();
	void finish_reading();
	void begin_writing(int32 data_size_alignment);
	void finish_writing(int32* out_space_left_in_bits);

	bool would_overflow(int32 bit_count)
	{
		return bit_count + m_current_bit_position > 8 * m_stream_buf_size_bytes;
	}

	bool overflowed() const
	{
		return m_current_bit_position > m_stream_buf_size_bytes * 8;
	}

	bool error_occured() const
	{
		bool result = overflowed();
		// debug mode in release mode should be disabled !!!
		if (m_error_stream_debug_mode_is_enabled)
			result = true;

		return result;
	}

	int32 get_space_left_in_bits() const
	{
		return 8 * m_stream_buf_size_bytes - m_current_bit_position;
	}

	int32 get_space_used_in_bytes() const
	{
		return (m_current_bit_position + 7) / 8;
	}

	void read_string_wchar(const char* name, void* string, int size_in_words);
	void write_string_wchar(const char* name, void* string_buffer, int size_in_words);
	void write_integer(const char* name, unsigned int value, unsigned int size_in_bits);
	int32 read_integer(const char* name, uint32 size_in_bits);
	void data_encode_bits(const char* name, void* data, unsigned int size_in_bits);
	void data_decode_bits(const char* name, void* data, int size_in_bits);
	void data_encode_bool(const char* name, bool value);
	bool data_decode_bool(const char* name);
	void data_decode_address(const char* name, void* address);
	void data_encode_quantized_real(const char* name, float value, float min_value, float max_value, int size_in_bits, char exact_midpoint);
	float data_decode_quantized_real(const char* name, float min_value, float max_value, int size_in_bits, char exact_midpoint);
	void data_encode_unit_vector(const char* name, real_vector3d* vector);
	void data_decode_unit_vector(const char* name, real_vector3d* out_vector);
	void data_encode_signed_integer(const char* name, int value, unsigned int size_in_bits);
	int data_decode_signed_integer(const char* name, unsigned int size_in_bits);
	void data_encode_axes(const char* name, real_vector3d* forward, real_vector3d* up);
	void data_decode_axes(const char* name, real_vector3d* out_forward, real_vector3d* out_up);
	void data_encode_vector(const char* name, real_vector3d* vector, float min_magnitude_value, float max_magnitude_value, int magnitude_size_in_bits);
	void data_decode_vector(const char* name, real_vector3d* out_vector, float min_magnitude_value, float max_magnitude_value, int magnitude_size_in_bits);
	void data_encode_flags(const char *name, unsigned long long value, int size_in_bits);
	unsigned long long data_decode_flags(const char *name, int size_in_bits);
};
CHECK_STRUCT_SIZE(c_bitstream, 52);
