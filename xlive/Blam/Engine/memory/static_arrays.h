#pragma once
#include "text/unicode.h"

template<typename t_storage_type, size_t k_count>
class c_static_array
{
public:
	size_t get_count(void) const { return k_count; }
	bool valid(int32 index) const { return index < k_count; }

	const t_storage_type& operator[](int32 index) const
	{
		ASSERT(valid(index));
		return m_storage[index];
	}

	t_storage_type& operator[](int32 index)
	{
		ASSERT(valid(index));
		return m_storage[index];
	}

protected:
	t_storage_type m_storage[k_count];
};

template<typename t_type, size_t k_maximum_count>
class c_static_stack
{
public:
	c_static_stack(void) : m_count(0) {}
	void clear(void) { m_count = 0; }
	bool valid(void) const { return m_count <= k_maximum_count; }
	void push(void) { ASSERT(!full()); m_count++; }
	bool full(void) const { ASSERT(valid()); return m_count == k_maximum_count; }
	bool empty(void) const { return m_count == 0; }
	int32 top(void) const { ASSERT(!empty()); return m_count - 1; }

	uint32 get_total_size(void)
	{
		return k_maximum_count * sizeof(t_type);
	}
	
	uint32 get_type_size(void)
	{
		return sizeof(t_type);
	}
	
	uint32 get_count(void)
	{
		return m_count;
	}
	
	const t_type& operator[](int32 index) const
	{
		ASSERT(VALID_INDEX(index, k_maximum_count));
		return m_data[index];
	}

	t_type& operator[](int32 index)
	{
		ASSERT(VALID_INDEX(index, k_maximum_count));
		return m_data[index];
	}

	t_type* get_top(void)
	{
		return &this->operator[](top());
	}

protected:
	int32 m_count;
	t_type m_data[k_maximum_count];
};

template<typename t_type, typename t_storage_type, size_t k_count>
class c_flags_no_init 
{
	static_assert(std::is_unsigned_v<t_storage_type>, "error: t_storage_type is not an unsigned integral type");
	static_assert(k_count > 0 && k_count <= (sizeof(t_storage_type) * 8), "error: k_count < 0 || k_count > (sizeof(t_storage_type) * 8)");

	bool valid_bit(t_type bit) const
	{
		return VALID_INDEX(bit, k_count);
	}

	// Checks if current value set for the bitflag is valid
	bool valid(void) const
	{
		return !TEST_FLAG(m_storage, ~MASK(k_count));
	}

public:

	void set(t_type bit, bool enable)
	{
		ASSERT(valid_bit(bit));

		if (bit < k_count)
		{
			if (enable)
			{
				m_storage |= FLAG(bit);
			}
			else
			{
				m_storage &= ~FLAG(bit);
			}
		}
		return;
	}

	const t_storage_type get_unsafe(void) const
	{
		return m_storage;
	}

	void set_unsafe(t_storage_type raw_bits)
	{
		m_storage = raw_bits;
		ASSERT(valid());
		return;
	}

	void clear(void)
	{
		m_storage = 0;
		return;
	}

	bool test(t_type bit) const
	{
		ASSERT(valid_bit(bit));
		return TEST_BIT(m_storage, bit);
	}

	// Untested
	bool test_range(t_type start_bit, t_type end_bit) const
	{
		ASSERT(valid_bit(start_bit) && valid_bit(end_bit) && (start_bit <= end_bit));

		t_type bit = (FLAG(end_bit - start_bit + 1) - 1) << start_bit;
		return TEST_BIT(m_storage, bit);
	}

	c_flags_no_init<t_type, t_storage_type, k_count> operator~(void) const
	{
		c_flags_no_init<t_type, t_storage_type, k_count> result;
		result = ~m_storage;
		return result;
	}

	c_flags_no_init<t_type, t_storage_type, k_count> operator&(c_flags_no_init<t_type, t_storage_type, k_count>& value) const
	{
		c_flags_no_init<t_type, t_storage_type, k_count> result;
		result = m_storage & value.m_storage;
		return result;
	}

	void operator=(t_storage_type value)
	{
		set_unsafe(value);
		return;
	}

	void operator|=(c_flags_no_init<t_type, t_storage_type, k_count>& value)
	{
		m_storage |= value.m_storage;
		ASSERT(valid());
		return;
	}

	bool operator!=(c_flags_no_init<t_type, t_storage_type, k_count>& value) const
	{
		return m_storage != value.m_storage;
	}

	bool operator==(c_flags_no_init<t_type, t_storage_type, k_count>& value) const
	{
		return m_storage == value.m_storage;
	}

protected:
	t_storage_type m_storage;
};

template<typename t_type, typename t_storage_type, size_t k_count>
class c_flags : public c_flags_no_init<t_type, t_storage_type, k_count>
{
public:
	c_flags(void) 
	{ 
		this->clear();
		return; 
	}

	c_flags(t_storage_type value)
	{
		*this = value;
		return;
	}

	void operator=(t_storage_type value)
	{
		c_flags_no_init<t_type, t_storage_type, k_count>::operator=(value);
	}
};

template<size_t k_maximum_count>
class c_static_flags_no_init
{
public:
	void clear(void)
	{
		csmemset(m_flags, 0, BIT_VECTOR_SIZE_IN_BYTES(k_maximum_count));
		return;
	}

	void clear_range(int32 count)
	{
		csmemset(m_flags, 0, BIT_VECTOR_SIZE_IN_BYTES(count));
		return;
	}

	void fill(int32 count, uint8 value)
	{
		csmemset(m_flags, value, BIT_VECTOR_SIZE_IN_BYTES(count));
		return;
	}

	const uint32* get_bits_direct(void) const
	{
		return m_flags;
	}

	uint32* get_writeable_bits_direct(void)
	{
		return m_flags;
	}

	void set(int32 index, bool enable)
	{
		ASSERT(VALID_INDEX(index, k_maximum_count));
		ASSERT(m_flags);

		BIT_VECTOR_SET_FLAG(m_flags, index, enable);
		return;
	}

	void set_bits_direct_destructive(int32 count, uint8 value)
	{
		csmemcpy(m_flags, value, BIT_VECTOR_SIZE_IN_BYTES(count));
		return;
	}

	bool test(int32 index) const
	{
		ASSERT(VALID_INDEX(index, k_maximum_count));
		ASSERT(m_flags);

		return BIT_VECTOR_TEST_FLAG(m_flags, index);
	}

protected:
	/* 
	We do : "+ (k_maximum_bit_count % LONG_BITS > 0)" so we can make sure we have enough space
	when m_flags is not perfectly divisible by LONG_BITS. It adds an extra element if the condition is true
	*/
	uint32 m_flags[k_maximum_count / LONG_BITS + (k_maximum_count % LONG_BITS > 0)];
};

template<size_t k_maximum_bit_count>
class c_static_flags : public c_static_flags_no_init<k_maximum_bit_count>
{
public:
	c_static_flags(void) = default;
	c_static_flags(int32 count, uint8 value)
	{
		this->fill(count, value);
		return;
	}
	~c_static_flags(void) = default;
};

template<size_t string_length>
struct c_static_string
{
public:
	c_static_string(void)
	{
		m_string[0] = '\0';
	}

	c_static_string(const char* string)
	{
		this->set(string);
	}

	~c_static_string() = default;

	char* clear(void);
	const char* get_string(void) const;
	char* get_buffer(void);
	char* set(const char* src);
	size_t length(void) const;
	size_t max_length(void) const;
	char* append(const char* src);
	int index_of(const char* src) const;
	int next_index_of(const char* src, size_t starting_index) const;
	int last_index_of(const char* src) const;
	bool is_equal(const char* string) const;
	const char* print(const char* format, ...);
	const char* append_print(const char* format, ...);

private:
	char m_string[string_length];
};

template<size_t string_length>
class c_static_wchar_string
{
public:
	c_static_wchar_string(void)
	{
		m_string[0] = L'\0';
	}

	c_static_wchar_string(const wchar_t* string)
	{
		this->set(string);
	}

	~c_static_wchar_string() = default;

	wchar_t* clear(void);
	const wchar_t* get_string(void) const;
	wchar_t* get_buffer(void);
	wchar_t* set(const wchar_t* src);
	size_t length(void) const;
	size_t max_length(void) const;
	wchar_t* append(const wchar_t* src);
	int index_of(const wchar_t* src) const;
	int next_index_of(const wchar_t* src, size_t starting_index) const;
	int last_index_of(const wchar_t* src) const;
	bool is_equal(const wchar_t* string) const;
	errno_t to_lower(void);
	const wchar_t* print(const wchar_t* format, ...);
	const wchar_t* append_print(const wchar_t* format, ...);

private:
	wchar_t m_string[string_length];
};

template<size_t T>
inline const char* c_static_string<T>::get_string(void) const
{
	return this->m_string;
}

template<size_t T>
inline char* c_static_string<T>::get_buffer(void)
{
	return this->m_string;
}

template<size_t T>
inline char* c_static_string<T>::clear(void)
{
	this->m_string[0] = '\0';
	return this->m_string;
}

template<size_t T>
inline char* c_static_string<T>::set(const char* src)
{
	return csstrncpy(this->get_buffer(), src, T);
}

template<size_t T>
inline size_t c_static_string<T>::length(void) const
{
	return csstrnlen(this->get_string(), T);
}

template<size_t T>
inline size_t c_static_string<T>::max_length(void) const
{
	return T;
}

template<size_t T>
char* c_static_string<T>::append(const char* src)
{
	char* result = csstrncat(this->get_buffer(), src, T);
	result[T - 1] = 0;
	return result;

}

// Gets the index of the specified substring
template<size_t T>
int c_static_string<T>::index_of(const char* src) const
{
	return this->next_index_of(src, 0);
}

// Gets the next index of the specified substring at the starting index
template<size_t T>
int c_static_string<T>::next_index_of(const char* src, size_t starting_index) const
{
	int index;
	const char* substring;

	index = -1;
	if (starting_index < this->length())
	{
		substring = strstr(this->get_string()[starting_index], src);
		if (substring)
		{
			index = substring - this->get_string();
		}
	}
	return index;
}

// Gets the last index of the specified substring
template<size_t T>
int c_static_string<T>::last_index_of(const char* src) const
{
	int index = -1;
	do
	{
		int temp_index = this->next_index_of(src, index + 1);
		if (temp_index != -1)
		{
			index = temp_index;
		}
		else
		{
			break;
		}
	} while (1);

	return index;
}

template<size_t T>
bool c_static_string<T>::is_equal(const char* string) const
{
	return !strncmp(m_string, string, T);
}

template<size_t T>
const char* c_static_string<T>::print(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	ASSERT(format);
	vsnprintf(m_string, T, format, args);

	va_end(args);
	return m_string;
}

template<size_t T>
const char* c_static_string<T>::append_print(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	const size_t current_length = this->length();

	ASSERT(format);
	ASSERT(current_length >= 0 && current_length < NUMBEROF(m_string));

	vsprintf(&m_string[current_length], T - current_length, format, args);
	va_end(args);
	return m_string;
}

template<size_t T>
inline const wchar_t* c_static_wchar_string<T>::get_string(void) const
{
	return this->m_string;
}

template<size_t T>
inline wchar_t* c_static_wchar_string<T>::get_buffer(void)
{
	return this->m_string;
}

template<size_t T>
inline wchar_t* c_static_wchar_string<T>::clear(void)
{
	this->m_string[0] = L'\0';
	return this->m_string;
}

template<size_t T>
inline wchar_t* c_static_wchar_string<T>::set(const wchar_t* src)
{
	return ustrncpy(this->get_buffer(), src, T);
}

template<size_t T>
inline size_t c_static_wchar_string<T>::length(void) const
{
	return ustrnlen(this->get_string(), T);
}

template<size_t T>
inline size_t c_static_wchar_string<T>::max_length(void) const
{
	return T;
}

template<size_t T>
wchar_t* c_static_wchar_string<T>::append(const wchar_t* src)
{
	wchar_t* result = ustrncat(this->get_buffer(), src, T);
	result[T - 1] = 0;
	return result;
}

// Gets the index of the specified substring
template<size_t T>
int c_static_wchar_string<T>::index_of(const wchar_t* src) const
{
	return this->next_index_of(src, 0);
}

// Gets the next index of the specified substring at the starting index
template<size_t T>
int c_static_wchar_string<T>::next_index_of(const wchar_t* src, size_t starting_index) const
{
	int index;
	const wchar_t* substring;

	index = -1;
	if (starting_index < this->length())
	{
		substring = wcsstr(&this->get_string()[starting_index], src);
		if (substring)
		{
			index = substring - this->get_string();
		}
	}
	return index;
}

// Gets the last index of the specified substring
template<size_t T>
int c_static_wchar_string<T>::last_index_of(const wchar_t* src) const
{
	int index = -1;
	do
	{
		int temp_index = this->next_index_of(src, index + 1);
		if (temp_index != -1)
		{
			index = temp_index;
		}
		else
		{
			break;
		}
	} while (1);

	return index;
}

template<size_t T>
bool c_static_wchar_string<T>::is_equal(const wchar_t* string) const
{
	return !ustrncmp(m_string, string, T);
}

template<size_t T>
errno_t c_static_wchar_string<T>::to_lower(void)
{
	return _wcslwr_s(m_string, T);
}

template<size_t T>
const wchar_t* c_static_wchar_string<T>::print(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	ASSERT(format);
	uvsnprintf(this->m_string, T, format, args);
	return this->m_string;
}

template<size_t T>
const wchar_t* c_static_wchar_string<T>::append_print(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);

	const size_t current_length = this->length();

	ASSERT(format);
	ASSERT(current_length >= 0 && current_length < NUMBEROF(m_string));

	uvsnprintf(&m_string[current_length], T - current_length, format, args);
	va_end(args);
	return m_string;
}

/* globals */

typedef c_static_wchar_string<512> c_maximum_interface_text;


#ifdef ASSERTS_ENABLED
extern c_static_string<256> g_static_string_assert_text;
#endif

/* public code */

template<typename T>
inline bool tmpl_IN_RANGE(T value, T a, T b)
{
	return IN_RANGE(value, a, b);
}
