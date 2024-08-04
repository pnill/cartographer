#pragma once
#include "text/unicode.h"

#define MAXIMUM_STRING_SIZE 262144

template<size_t string_length>
struct c_static_string
{
public:
	c_static_string(void)
	{
		text[0] = '\0';
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

private:
	char text[string_length];
};

template<size_t string_length>
class c_static_wchar_string
{
public:
	c_static_wchar_string(void)
	{
		text[0] = L'\0';
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

private:
	wchar_t text[string_length];
};

typedef c_static_string<32>  static_string32;
typedef c_static_string<64>  c_static_string64;
typedef c_static_string<128> static_string128;
typedef c_static_string<256> static_string256;
typedef c_static_string<260> c_static_string260;
typedef c_static_string<512> static_string512;
typedef c_static_wchar_string<32>  c_static_wchar_string32;
typedef c_static_wchar_string<64>  c_static_wchar_string64;
typedef c_static_wchar_string<128> c_static_wchar_string128;
typedef c_static_wchar_string<256> c_static_wchar_string256;
typedef c_static_wchar_string<260> c_static_wchar_string260;

template<size_t T>
inline const char* c_static_string<T>::get_string(void) const
{
	return this->text;
}

template<size_t T>
inline char* c_static_string<T>::get_buffer(void)
{
	return this->text;
}

template<size_t T>
inline char* c_static_string<T>::clear(void)
{
	this->text[0] = '\0';
	return this->text;
}

template<size_t T>
inline char* c_static_string<T>::set(const char* src)
{
	return csstrnzcpy(this->get_buffer(), src, T);
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
	char* result = csstrnzcat(this->get_buffer(), src, T);
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
	return !strncmp(text, string, T);
}

template<size_t T>
inline const wchar_t* c_static_wchar_string<T>::get_string(void) const
{
	return this->text;
}

template<size_t T>
inline wchar_t* c_static_wchar_string<T>::get_buffer(void)
{
	return this->text;
}

template<size_t T>
inline wchar_t* c_static_wchar_string<T>::clear(void)
{
	this->text[0] = L'\0';
	return this->text;
}

template<size_t T>
inline wchar_t* c_static_wchar_string<T>::set(const wchar_t* src)
{
	return ustrnzcpy(this->get_buffer(), src, T);
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
	wchar_t* result = ustrnzcat(this->get_buffer(), src, T);
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
	} 
	while (1);

	return index;
}

template<size_t T>
bool c_static_wchar_string<T>::is_equal(const wchar_t* string) const
{
	return !ustrncmp(text, string, T);
}

template<size_t T>
errno_t c_static_wchar_string<T>::to_lower(void)
{
	return _wcslwr_s(text, T);
}

size_t csstrnlen(const char* s, size_t size);

char* csstrnzcpy(char* s1, const char* s2, size_t size);

char* csstrnzcat(char* s1, char const* s2, size_t size);


int32 csstricmp(const char* s1, const char* s2);

int32 csstrncmp(const char* s1, const char* s2, size_t size);

int32 cvsnzprintf(char* buffer, size_t size, char const* format, ...);