#pragma once

#define CHECK_ARG_STR(x,y) \
	if( strstr( str,x ) == str ) \
	{ \
		sscanf( str + strlen(x), "%s", &y ); \
		continue; \
	}

#define CHECK_ARG(x,y) \
	if( strstr( str,x ) == str ) \
	{ \
		sscanf( str + strlen(x), "%d", &y ); \
		continue; \
	}

#define CHECK_ARG_FLOAT(x,y) \
	if( strstr(str, x) == str) \
	{ \
		sscanf(str + strlen(x), "%f", &y ); \
		continue; \
	}

#define CHECK_ARG_I64(x,y) \
	if( strstr( str,x ) == str ) \
	{ \
		sscanf( str + strlen(x), "%I64x", &y ); \
		continue; \
	}

#define gCHECK_ARG(x,y) \
	if( strstr( gstr,x ) == gstr ) \
	{ \
		sscanf( gstr + strlen(x), "%d", &y ); \
		continue; \
	}