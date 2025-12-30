#pragma once

/* prototypes */

#if ERRORS_ENABLED

void rasterizer_dx9_errors_log(HRESULT hr, const char* format, ...);

#endif

/* macros */

#if ERRORS_ENABLED
#define rasterizer_dx9_log(statement)				\
do													\
{													\
	HRESULT hr = (statement);						\
	if (FAILED(hr))									\
	{												\
		rasterizer_dx9_errors_log(hr, (#statement));\
	}												\
}													\
while(0)


#define rasterizer_dx9_log_hr(hr, statement)		\
do													\
{													\
	hr = (statement);								\
	if (FAILED(hr))									\
	{												\
		rasterizer_dx9_errors_log(hr, (#statement));\
	}												\
}													\
while (0)

#else
#define rasterizer_dx9_log(statement) statement
#define rasterizer_dx9_log_hr(hr, statement) ((hr) = (statement))
#endif


