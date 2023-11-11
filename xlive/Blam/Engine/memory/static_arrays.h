#pragma once

template<typename t_type, typename t_storage_type, size_t k_count>
class c_flags
{
public:
	c_flags() : m_storage(0)
	{
		return;
	}

	c_flags(t_storage_type raw_bits) : m_storage(raw_bits)
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
