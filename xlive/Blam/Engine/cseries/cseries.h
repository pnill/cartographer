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

#define SIZEOF_BITS(value) 8 * sizeof(value)
#define CHAR_BITS SIZEOF_BITS(int8)
#define SHORT_BITS SIZEOF_BITS(int16)
#define LONG_BITS SIZEOF_BITS(int32)

// Invokes a function
// ADDR_CLIENT: file offset in halo2.exe
// ADDR_SERVER: file offset in h2server.exe
// TYPE: function
// __VA_ARGS__: arguments for the function we want to invoke
#define INVOKE(ADDR_CLIENT, ADDR_SERVER, TYPE, ...) Memory::GetAddress<decltype(TYPE)*>(ADDR_CLIENT, ADDR_SERVER)(__VA_ARGS__)

#define NONE -1
#define DATUM_INDEX_NONE ((datum)(NONE))
#define DATUM_INDEX_NEW(_absolute_index, _salt) (datum)((_absolute_index) | ((_salt) << 16))
#define DATUM_IS_NONE(_datum_index) ((_datum_index) == DATUM_INDEX_NONE)
#define DATUM_INDEX_TO_ABSOLUTE_INDEX(_datum_index) ((uint16)((_datum_index) & 0xFFFF))
#define DATUM_INDEX_TO_IDENTIFIER(_datum_index) ((uint16)(((_datum_index) >> 16) & 0xFFFF))

#define NUMBEROF(_array) (sizeof(_array) / sizeof(_array[0]))
#define IN_RANGE_INCLUSIVE(value, begin, end) (((value) >= (begin)) && ((value) <= (end)))
#define VALID_INDEX(index, count) ((index) >= 0 && (index) < (count))
#define VALID_COUNT(index, count) ((index) >= 0 && (index) <= (count))

// TODO remove padding macros
// Explanation:
// Harder to debug memory with it since types dont show up in the watch view
// Some values assigned to these macros actually contain data but are assigned incorrectly as padding

// PADDING MACROS
#define J( symbol1, symbol2 ) _DO_JOIN( symbol1, symbol2 )
#define _DO_JOIN( symbol1, symbol2 ) symbol1##symbol2
// Pad the desired length of BYTES and also hides it
#define PAD(BYTES) private: BYTE J(Unused_, __LINE__ )[BYTES];  public:       

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

/// Use this for setting up enum bitfields
#define FLAG(bit)( 1<<(bit) )
#define TEST_BIT(flags, bit)( ((flags) & FLAG(bit)) != 0 )
#define TEST_FLAG(flags, flag)( ((flags) & (flag)) != 0 )
#define SET_FLAG(flags, bit, value)( (value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)) )
#define SWAP_FLAG(flags, bit)			( (flags) ^=FLAG(bit) )
#define FLAG_RANGE(first_bit, last_bit)	( (FLAG( (last_bit)+1 - (first_bit) )-1) << (first_bit) )

#define BIT_VECTOR_SIZE_IN_LONGS(BIT_COUNT) (((BIT_COUNT) + (LONG_BITS - 1)) / LONG_BITS)
#define BIT_VECTOR_SIZE_IN_BYTES(BIT_COUNT) (4 * BIT_VECTOR_SIZE_IN_LONGS(BIT_COUNT))
#define BIT_VECTOR_TEST_FLAG(BIT_VECTOR, BIT) (TEST_BIT(BIT_VECTOR[(BIT) / LONG_BITS], ((BIT) & (LONG_BITS - 1))))
#define BIT_VECTOR_SET_FLAG(BIT_VECTOR, BIT, ENABLE) (SET_FLAG(BIT_VECTOR[(BIT) / LONG_BITS], ((BIT) & (LONG_BITS - 1)), ENABLE))

/// Creates a mask out of a count number of flags
#define MASK(count) ( (unsigned)(1 << (count)) - (unsigned)1 )

#define CHECK_STRUCT_SIZE(STRUCT,_SIZE)\
static_assert (sizeof(STRUCT) == _SIZE, "Invalid size for struct (" #STRUCT")")

#define CHECK_STRUCT_OFFSET(STRUCT,FIELD,OFFSET)\
static_assert (offsetof(STRUCT, FIELD) == OFFSET, #STRUCT " layout is invalid")

#define TAG_BLOCK_SIZE_ASSERT(tagblock,size)\
static_assert (sizeof(tagblock) == (size),"Invalid Size for TagBlock <" #tagblock ">")

#define TAG_GROUP_SIZE_ASSERT(tagGroup,size)\
static_assert (sizeof(tagGroup) == (size),"Invalid Size for TagGroup <" #tagGroup">");


// TODO reimplement this properly
void* csmemset(void* dst, int32 val, size_t size);

// TODO reimplement this properly
void* csmemcpy(void* dst, const void* src, size_t size);
