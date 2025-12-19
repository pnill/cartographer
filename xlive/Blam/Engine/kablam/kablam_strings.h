#pragma once

#define kablam_string_quick_wprintf(fmt, string_id) \
do													\
{													\
	if(string_id)									\
	{												\
		kablam_string __str(string_id);				\
		wprintf(fmt, __str.get());					\
		__str.free();								\
	}												\
} while(0)

class kablam_string
{
	HANDLE string_handle;
	const wchar_t* string;
	bool string_found;
	bool string_is_resource_handle;

public:
	kablam_string();
	kablam_string(int32 string_id);
	errno_t load(int32 string_id);
	const wchar_t* get() const;
	void free();
	static short* load_resource(int32 string_id, WORD language);
};
