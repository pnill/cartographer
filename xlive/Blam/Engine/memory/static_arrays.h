#pragma once

template<typename t_type, typename t_storage_type, size_t k_count>
class c_flags
{
public:
	c_flags() 
		: m_storage(0)
	{
		return;
	}

	c_flags(t_storage_type raw_bits) 
		: m_storage(raw_bits)
	{
		return;
	}

	void set_raw_bits(t_storage_type raw_bits)
	{
		m_storage = raw_bits;
		return;
	}

	void set(t_type bit, bool enable)
	{
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

	void clear()
	{
		m_storage = 0;
		return;
	}

	t_storage_type not()
	{
		return ~m_storage;
	}

	void or(t_storage_type flags)
	{
		m_storage |= flags;
	}

	bool valid_bit(t_type bit) const
	{
		return VALID_INDEX(0, k_count);
	}

	bool test(t_type bit) const
	{
		return TEST_BIT(m_storage, (t_storage_type)(bit));
	}

	bool operator==(c_flags<t_type, t_storage_type, k_count>& value)
	{
		return m_storage == value.m_storage;
	}

	bool operator==(t_type value)
	{
		return !!(m_storage & FLAG(value));
	}

	template <class T>
	void operator= (T value)
	{
		m_storage = (t_storage_type)(value);
		return;
	}

	template <class T>
	operator T () const
	{
		return (T)(m_storage);
	}

protected:
	t_storage_type m_storage;
};

template<size_t k_maximum_bit_count>
class c_static_flags_no_init
{
public:
	void clear(void)
	{
		csmemset(m_flags, 0, BIT_VECTOR_SIZE_IN_BYTES(k_maximum_bit_count));
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

	const int32* get_bits_direct(void) const
	{
		return m_flags;
	}

	int32* get_writeable_bits_direct(void)
	{
		return m_flags;
	}

	void set(int32 index, bool enable)
	{
		BIT_VECTOR_SET_FLAG(m_flags, index, enable);
		return;
	}

	void set_bits_direct_destructive(int32 count, uint8 value)
	{
		csmemcpy(m_flags, value, BIT_VECTOR_SIZE_IN_BYTES(count));
		return;
	}

	bool test(int32 index)
	{
		return BIT_VECTOR_TEST_FLAG(m_flags, index);
	}

protected:
	int32 m_flags[k_maximum_bit_count / LONG_BITS];
};

template<size_t k_maximum_bit_count>
class c_static_flags : public c_static_flags_no_init<k_maximum_bit_count>
{
public:
	c_static_flags(void) = default;
	c_static_flags(int32 count, uint8 value)
	{
		fill(count, value);
		return;
	}
	~c_static_flags(void) = default;
};
