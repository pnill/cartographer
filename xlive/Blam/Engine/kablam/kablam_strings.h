#pragma once

/* macros */

#define kablam_string_quick_wprintf(fmt, string_id) \
do													\
{													\
	kablam_string __str(string_id);				\
	wprintf(fmt, __str.get());					\
	__str.free();								\
} while(0)

/* classes */

class kablam_string
{
	HANDLE m_string_handle;
	wchar_t const* m_string;
	bool m_string_found;
	bool m_string_is_resource_handle;

public:
	kablam_string();
	kablam_string(int32 string_id);
	errno_t load(int32 string_id);
	wchar_t const* get() const;
	void free();
	static short* load_resource(int32 string_id, WORD language);
};
