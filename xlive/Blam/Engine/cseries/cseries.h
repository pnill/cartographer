#pragma once

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

#define NUMBEROF(_array) (sizeof(_array) / sizeof(_array[0]))
#define IN_RANGE_INCLUSIVE(value, begin, end) (((value) >= (begin)) && ((value) <= (end)))
#define VALID_INDEX(index, count) ((index) >= 0 && (index) < (count))
#define VALID_COUNT(index, count) ((index) >= 0 && (index) <= (count))
