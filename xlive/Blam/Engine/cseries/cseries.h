#pragma once

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

// 32-bit file storage identifier
typedef int32 enumerated_file_index;
static_assert(sizeof(enumerated_file_index) == 4);

// 32-bit character that's specified as a utf32 string
struct utf32
{
	uint32 character;
};


enum
{
	k_kilo = 1024,
	SHORT_MAX = 32767,
	k_unsigned_short_max = 0xffff
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

/* globals */

extern char g_temporary[256];

extern bool g_catch_exceptions;

/* macros */

#define NONE (-1)
#define DATUM_INDEX_NEW(_absolute_index, _salt) (datum)(((_salt) << 16) | (_absolute_index))
#define DATUM_INDEX_TO_ABSOLUTE_INDEX(_datum_index) ((uint16)((_datum_index) & 0xFFFF))
#define DATUM_INDEX_TO_IDENTIFIER(_datum_index) ((uint16)(((_datum_index) >> 16) & 0xFFFF))

#define NUMBEROF(_array) (sizeof(_array) / sizeof(*_array))
#define IN_RANGE(value, begin, end) (((value) >= (begin)) && ((value) <= (end)))
#define VALID_INDEX(index, count) ((index) >= 0 && (index) < (count))
#define VALID_COUNT(index, count) ((index) >= 0 && (index) <= (count))

#define ENUMERATED_INDEX_IS_DEFAULT_SAVE(enumerated_file_index) (((enumerated_file_index) & 0x200000) != 0)
#define ENUMERATED_INDEX_GET_TYPE(enumerated_file_index) (e_saved_game_file_type)(((enumerated_file_index) & 0xF) >> 0)
#define ENUMERATED_INDEX_GET_ABS_INDEX(enumerated_file_index) (((enumerated_file_index) & 0x1FFF00) >> 8)
#define ENUMERATED_INDEX_GET_MEMORY_UNIT(enumerated_file_index)	(((enumerated_file_index) & 0xF0) >> 4)
#define ENUMERATED_INDEX_GET_SALT(enumerated_file_index) (((enumerated_file_index) & 0x7FC000 ) >> 14)

// TODO remove padding macros
// Explanation:
// Harder to debug memory with it since types dont show up in the watch view
// Some values assigned to these macros actually contain data but are assigned incorrectly as padding

// PADDING MACROS
#define J( symbol1, symbol2 ) _DO_JOIN( symbol1, symbol2 )
#define _DO_JOIN( symbol1, symbol2 ) symbol1##symbol2
// Pad the desired length of BYTES and also hides it
#define PAD(BYTES)  char J(Unused_, __LINE__ )[BYTES];       

/// Add an anonymous 8-bit (1 byte) field to a structure.
#define PAD8 unsigned char : 8;
/// Add an anonymous 16-bit (2 byte) field to a structure.
#define PAD16 unsigned short : 16;
/// Add an anonymous 24-bit field to a structure.
#define PAD24 unsigned char : 8; unsigned short : 16;
/// Add an anonymous 32-bit (4 byte) field to a structure.
#define PAD32 unsigned long : 32;
/// Add an anonymous 48-bit field to a structure.
#define PAD48 unsigned short : 16; unsigned long : 32;
/// Add an anonymous 64-bit (8 byte) field to a structure.
#define PAD64 unsigned __int64 : 64;
/// Add an anonymous 128-bit (16 byte) field to a structure.
#define PAD128 unsigned __int64 : 64; unsigned __int64 : 64;

// Use this for setting up enum bitfields
#define FLAG(bit) ( (unsigned)1 << (unsigned)(bit) )
#define TEST_BIT(flags, bit)( ((flags) & FLAG(bit)) != 0 )
#define TEST_FLAG(flag, flags)( ((flag) & (flags)) != 0 )
#define SET_FLAG(flags, bit, value)( (value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)) )
#define SWAP_FLAG(flags, bit)			( (flags) ^=FLAG(bit) )
#define FLAG_RANGE(first_bit, last_bit)	( (FLAG( (last_bit)+1 - (first_bit) )-1) << (first_bit) )

#define BIT_VECTOR_SIZE_IN_LONGS(BIT_COUNT) (((BIT_COUNT) + (LONG_BITS - 1)) / LONG_BITS)
#define BIT_VECTOR_SIZE_IN_BYTES(BIT_COUNT) (4 * BIT_VECTOR_SIZE_IN_LONGS(BIT_COUNT))
#define BIT_VECTOR_TEST_FLAG(BIT_VECTOR, BIT) (TEST_BIT(BIT_VECTOR[(BIT) / LONG_BITS], ((BIT) & (LONG_BITS - 1))))
#define BIT_VECTOR_SET_FLAG(BIT_VECTOR, BIT, ENABLE) (SET_FLAG(BIT_VECTOR[(BIT) / LONG_BITS], ((BIT) & (LONG_BITS - 1)), ENABLE))

// Creates a bitmask that sets every bit between (bit) - 1 and bit 0 to 1
// It does this by taking the FLAG value of bit - 1 and using logical OR on it with one of two choices:
// 0 if the bit passed is less than or equal to 1, otherwise 1 minus the FLAG value of bit - 1
// Ex. MASK(12) sets bit 11 to bit 0 to 1
#define MASK(bit) ( (FLAG((bit)-1)) | ((bit) <= 1 ? 0 : ( (FLAG((bit)-1) - 1) )) )

#define ASSERT_STRUCT_SIZE(STRUCT, _SIZE)\
static_assert (sizeof(STRUCT) == (_SIZE), "Invalid size for struct ("#STRUCT") expected size (" #_SIZE")");

#define ASSERT_STRUCT_OFFSET(STRUCT,FIELD,OFFSET)\
static_assert (offsetof(STRUCT, FIELD) == (OFFSET), #STRUCT " Offset(" #OFFSET ") for " #FIELD " is invalid");

#ifdef ASSERTS_ENABLED
#define ASSERT_TRIGGER_EXCEPTION(IS_EXCEPTION)	\
if (IS_EXCEPTION)								\
{												\
	if (is_debugger_present())					\
		__debugbreak();							\
	else if (!g_catch_exceptions)				\
		exit(-1);								\
}												\
else											\
{												\
	if (is_debugger_present())					\
		__debugbreak();							\
}												\
(void)0

#define DISPLAY_ASSERT_EXCEPTION(STATEMENT, IS_EXCEPTION)		\
display_assert(STATEMENT, __FILE__, __LINE__, IS_EXCEPTION);	\
ASSERT_TRIGGER_EXCEPTION(IS_EXCEPTION);							\
(void)0

#define DISPLAY_ASSERT(STATEMENT)			\
DISPLAY_ASSERT_EXCEPTION(STATEMENT, true);	\
(void)0

#define ASSERT_EXCEPTION(STATEMENT, IS_EXCEPTION)               \
if (!(STATEMENT))                                               \
{																\
	DISPLAY_ASSERT_EXCEPTION(#STATEMENT, IS_EXCEPTION);			\
}                                                               \
(void)0

#define ASSERT(STATEMENT)			\
ASSERT_EXCEPTION(STATEMENT, true);	\
(void)0

#define unreachable() DISPLAY_ASSERT("unreachable")

#else
#define ASSERT_TRIGGER_EXCEPTION()							(void)(0)
#define DISPLAY_ASSERT_EXCEPTION(STATEMENT, IS_EXCEPTION)   (void)(#STATEMENT)
#define DISPLAY_ASSERT(STATEMENT)                           (void)(#STATEMENT)
#define ASSERT_EXCEPTION(STATEMENT, IS_EXCEPTION)           (void)(#STATEMENT)
#define ASSERT(STATEMENT)                                   (void)(#STATEMENT)

#define unreachable() (void)(0)

#endif // _DEBUG

/* prototypes */

void cseries_initialize(void);

#ifdef ASSERTS_ENABLED
// TODO implement
void display_assert(char const* condition, char const* file, int32 line, bool assertion_failed);
#endif

void* csmemmove(void* destination, void* source, size_t size);

void memmove_guarded(void* write_start, const void* src, size_t size, void* bounds_lower, size_t bounds_size);

void* csmemset(void* destination, int32 val, size_t size);

void* csmemcpy(void* destination, const void* source, size_t size);

int csmemcmp(const void* p1, const void* p2, size_t size);

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

char* csstrncat(char* s1, char const* s2, size_t size);

size_t cstrlen(const char* s);

char* csstrnupr(char* s, size_t size);

int32 csstricmp(const char* s1, const char* s2);

int32 csstrncmp(const char* s1, const char* s2, size_t size);

// Convert string to lowercase
char* csstrnlwr(char* s, size_t size);

constexpr uint32 bits_required_for(uint64 max_value)
{
	// minimum of 1 bit for 1 and 0

	uint32 bits = 1;
	while (max_value > 1)
	{
		max_value >>= 1;
		++bits;
	}
	return bits;
}