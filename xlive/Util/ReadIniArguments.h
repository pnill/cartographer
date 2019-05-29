#pragma once

#define CHECK_ARG_STR(x,y) \
	if( strstr( str,x ) == str ) \
	{ \
		sscanf( str + strlen(x), "%s", (const char*)&y ); \
		continue; \
	}

#define CHECK_ARG(x,y) \
	if( strstr( str,x ) == str ) \
	{ \
		sscanf( str + strlen(x), "%d", (int*)&y ); \
		continue; \
	}

#define CHECK_ARG_BOOL(x,y) \
	if( strstr( str,x ) == str ) \
	{ \
		sscanf( str + strlen(x), "%d", (int*)((bool*)&y) ); \
		continue; \
	}

#define CHECK_ARG_FLOAT(x,y) \
	if( strstr(str, x) == str) \
	{ \
		sscanf(str + strlen(x), "%f", (float*)&y ); \
		continue; \
	}

#define CHECK_ARG_INT(x,y) \
	if( strstr(str, x) == str) \
	{ \
		sscanf(str + strlen(x), "%i", (int*)&y ); \
		continue; \
	}

#define CHECK_ARG_I64(x,y) \
	if( strstr( str,x ) == str ) \
	{ \
		sscanf( str + strlen(x), "%I64x", (long long*)&y ); \
		continue; \
	}