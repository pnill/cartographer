#pragma once
#include <stdlib.h>

// 8-bit character that's specified as a multibyte utf8 string
typedef char utf8;

// 8-bit signed integer ranging from -128 to 127
typedef signed char int8;
static_assert(sizeof(int8) == 1);

// 8-bit unsigned integer ranging from 0 to 255
typedef unsigned char uint8;
static_assert(sizeof(uint8) == 1);

// 16-bit signed integer ranging from -32,768 to 32,767
typedef signed __int16 int16;
static_assert(sizeof(int16) == 2);

// 16-bit unsigned integer ranging from 0 to 65,535
typedef unsigned __int16 uint16;
static_assert(sizeof(uint16) == 2);

// 32-bit signed integer ranging from -2,147,483,648 to 2,147,483,647
typedef signed long int32;
static_assert(sizeof(int32) == 4);

// 32-bit unsigned integer ranging from 0 to 4,294,967,295
typedef unsigned long uint32;
static_assert(sizeof(uint32) == 4);

// 64-bit signed integer ranging from -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807
typedef signed long long int64;
static_assert(sizeof(int64) == 8);

// 64-bit unsigned integer ranging from 0 to 18,446,744,073,709,551,615
typedef unsigned long long uint64;
static_assert(sizeof(uint64) == 8);

// 32-bit float
typedef float real32;
static_assert(sizeof(real32) == 4);

// 64-bit float
typedef double real64;
static_assert(sizeof(real64) == 8);

// 32-bit identifier: 16-bit index, 16-bit pseudounique "salt"
typedef long datum;
static_assert(sizeof(datum) == 4);

// 32-bit character that's specified as a utf32 string
struct utf32
{
	uint32 character;
};

enum
{
	BYTE_MAX = 255,
	k_kilo = 1024,
	SHORT_MAX = 32767,
	k_unsigned_short_max = 0xffff,
	NONE = -1,

	k_tag_string_length = 32,
};

#define EOL_STRING "\r\n"

#define SIZEOF_BITS(value) (8 * sizeof(value))
#define CHAR_BITS SIZEOF_BITS(int8)
#define SHORT_BITS SIZEOF_BITS(int16)
#define LONG_BITS SIZEOF_BITS(int32)

#ifdef LLVM
#define LLVM_JMP_ERROR static_assert(false, "JUMPING TO VIRTUAL FUNCTIONS IS BROKEN WHEN COMPILING IN LLVM: WE CANNOT MAKE A PROPER BUILD WITHOUT FIXING THIS")
#else
#define LLVM_JMP_ERROR (void)0
#endif

/* structures */

struct csstrtok_data
{
	char* next_string;
};

/* globals */

#ifdef ASSERTS_ENABLED
extern char g_temporary[256];

extern bool g_catch_exceptions;
#endif

/* macros */

#define TEST_N_DEF(TEST)

#define STRINGIFY(x) #x

#define J( symbol1, symbol2 ) _DO_JOIN( symbol1, symbol2 )
#define _DO_JOIN( symbol1, symbol2 ) symbol1##symbol2

// Returns the minimum of the two values
#define MIN(a, b) ((a)>(b)?(b):(a))

// Returns the maximum of the two values
#define MAX(a, b) ((a)>(b)?(a):(b))

// Ensure a value stays within a certain range
#define PIN(v, v_min, v_max) MAX(v_min, MIN(v, v_max))

#define CLAMP_LOWER(x, low, high) ((x) >= (high) - (low) ? (x) - (high) : (low))

#define CLAMP_UPPER(x, low, high) ((x) <= (high) - (low) ? (x) + (low) : (high))

#define DATUM_INDEX_NEW(_absolute_index, _salt) (datum)(((_salt) << 16) | (_absolute_index))
#define DATUM_INDEX_TO_ABSOLUTE_INDEX(_datum_index) ((uint16)((_datum_index) & k_unsigned_short_max))
#define DATUM_INDEX_TO_IDENTIFIER(_datum_index) ((uint16)((_datum_index) >> 16))

#define NUMBEROF(_array) (sizeof(_array) / sizeof(*_array))
#define IN_RANGE(value, begin, end) (((value)>=(begin)) && ((value)<=(end)))
#define VALID_INDEX(index, count) ((index)>=0 && (index)<(count))
#define VALID_COUNT(index, count) ((index)>=0 && (index)<=(count))

// Use this for setting up enum bitfields
#define FLAG(bit) ( (unsigned)1 << (unsigned)(bit) )
#define TEST_BIT(flags, bit)( ((flags) & FLAG(bit)) != 0 )
#define TEST_FLAG(flag, flags)( ((flag) & (flags)) != 0 )
#define SET_BIT(flags, bit, value)( (value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)) )
#define SWAP_FLAG(flags, bit)			( (flags) ^=FLAG(bit) )
#define FLAG_RANGE(first_bit, last_bit)	( (FLAG( (last_bit)+1 - (first_bit) )-1) << (first_bit) )

// Make sure no bits outside of the bit count are set within the mask
#define VALID_BITS(mask, bit_count) (!((mask) >> bit_count))

#define BIT_VECTOR_SIZE_IN_LONGS(BIT_COUNT) (((BIT_COUNT) + (LONG_BITS - 1)) / LONG_BITS)
#define BIT_VECTOR_SIZE_IN_BYTES(BIT_COUNT) (4 * BIT_VECTOR_SIZE_IN_LONGS(BIT_COUNT))
#define BIT_VECTOR_TEST_FLAG(BIT_VECTOR, BIT) (TEST_BIT(BIT_VECTOR[(BIT) / LONG_BITS], ((BIT) & (LONG_BITS - 1))))
#define BIT_VECTOR_SET_FLAG(BIT_VECTOR, BIT, ENABLE) (SET_BIT(BIT_VECTOR[(BIT) / LONG_BITS], ((BIT) & (LONG_BITS - 1)), ENABLE))

// Creates a bitmask that sets every bit between (bit) - 1 and bit 0 to 1
// Also can offset the resulting mask, if you need a specific bit region to be masked
// Ex. MASK(12) sets bit 11 through bit 0 to 1

#define MASK_OFFSET(count, offset) ( ( FLAG((count) - 1) | (FLAG((count) - 1) - 1) ) << (offset) )
#define MASK(count) MASK_OFFSET(count, 0)

#define ASSERT_STRUCT_SIZE(STRUCT, _SIZE)\
static_assert (sizeof(STRUCT) == (_SIZE), "Invalid size for struct ("#STRUCT") expected size (" #_SIZE")");

#define ASSERT_STRUCT_OFFSET(STRUCT,FIELD,OFFSET)\
static_assert (offsetof(STRUCT, FIELD) == (OFFSET), #STRUCT " Offset(" #OFFSET ") for " #FIELD " is invalid");

#ifdef ASSERTS_ENABLED

#define DISPLAY_ASSERT_EXCEPTION(STATEMENT, FILE, LINE, IS_EXCEPTION)	\
do																		\
{																		\
	display_assert(STATEMENT, FILE, LINE, IS_EXCEPTION);				\
	if (is_debugger_present())											\
		__debugbreak();													\
	else if (!g_catch_exceptions)										\
		exit(-1);														\
}																		\
while(0)

#define DISPLAY_ASSERT(STATEMENT) DISPLAY_ASSERT_EXCEPTION(STATEMENT, __FILE__, __LINE__, true)


#define ASSERT_EXCEPTION(STATEMENT, IS_EXCEPTION)								\
do																				\
{																				\
	if (!(STATEMENT))															\
	{																			\
		DISPLAY_ASSERT_EXCEPTION(#STATEMENT, __FILE__, __LINE__, IS_EXCEPTION);	\
	}																			\
}																				\
while(0)

#define ASSERT(STATEMENT) ASSERT_EXCEPTION(STATEMENT, true)

#define assert_return(pointer) assert_return_internal(pointer, #pointer, __FILE__, __LINE__)

#define vassert(STATEMENT, FORMAT, ...)	\
do																								\
{																								\
	if (!(STATEMENT))																			\
	{																							\
		DISPLAY_ASSERT(csprintf(g_temporary, NUMBEROF(g_temporary), (FORMAT), __VA_ARGS__));	\
	}																							\
}																								\
while(0)

#define unreachable() DISPLAY_ASSERT("unreachable")

#define halt() DISPLAY_ASSERT("halt()")

#else
#define DISPLAY_ASSERT_EXCEPTION(STATEMENT, IS_EXCEPTION)   (void)(#STATEMENT)
#define DISPLAY_ASSERT(STATEMENT)                           (void)(#STATEMENT)
#define ASSERT_EXCEPTION(STATEMENT, IS_EXCEPTION)           __assume(STATEMENT)
#define ASSERT(STATEMENT)                                   __assume(STATEMENT)

#define assert_return(pointer)		assert_return_internal(pointer)
#define vassert(STATEMENT, ...)		__assume((STATEMENT))
#define unreachable()				__assume(0)
#define halt()						__assume(0)

#endif // ASSERTS_ENABLED

/* prototypes */

extern bool is_debugger_present(void);

void cseries_initialize(void);

#ifdef ASSERTS_ENABLED
void display_assert(char const* condition, char const* file, int32 line, bool assertion_failed);
#endif

void* csmemmove(void* destination, void* source, size_t size);

void memmove_guarded(void* write_start, const void* src, size_t size, void* bounds_lower, size_t bounds_size);

void* csmemset(void* destination, int32 val, size_t size);

void* csmemcpy(void* destination, const void* source, size_t size);

int32 csmemcmp(const void* p1, const void* p2, size_t size);

int32 ascii_stricmp(const char* s1, const char* s2);

int32 csstrnicmp(const char* s1, const char* s2, size_t size);

int32 csstricmp(const char* s1, const char* s2);

int32 vsprintf(char* buffer, size_t size, const char* format, char* ap);

/* 
* NOTES:
* size_t max_count was added in vista to this function call
*/
int32 vsnprintf(char* buffer, size_t size, size_t max_count, const char* format, char* ap);


const char* csprintf(char* buffer, size_t size, const char* format, ...);

/*
* NOTES:
* size_t max_count was added in vista to this function call
*/
const char* csnprintf(char* buffer, size_t size, size_t max_count, const char* format, ...);


size_t csstrnlen(const char* s, size_t size);

char* csnappendf(char* s, size_t size, const char* format, ...);

char* csstrncpy(char* s1, const char* s2, size_t size);

wchar_t* ustrncpy_debug(wchar_t* s1, const wchar_t* s2, size_t size);

char* csstrncat(char* s1, char const* s2, size_t size);

size_t cstrlen(const char* s);

char* csstrnupr(char* s, size_t size);

int32 csstrcmp(const char* s1, const char* s2);

int32 csstrncmp(const char* s1, const char* s2, size_t size);

// Convert string to lowercase
char* csstrnlwr(char* s, size_t size);

char* csstrtok(char* s, const char* delimiters, bool skip_multiple_delimiters, struct csstrtok_data *data);

char* strchr(char* str, int32 ch);

/* public code */

template <typename T>
#ifdef ASSERTS_ENABLED
T* assert_return_internal(T* pointer, const char* statement, const char* file, int32 line)
{
	if (!pointer)
	{
		DISPLAY_ASSERT_EXCEPTION(statement, file, line, true);
	}
	return pointer;
}
#else
__forceinline T* assert_return_internal(T* pointer)
{
	__assume(pointer);
	return pointer;
}
#endif
