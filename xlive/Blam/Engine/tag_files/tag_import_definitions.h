#pragma once
#include "Blam/Cache/DataTypes/DataRef.h"

#define MAXIMUM_FILES_PER_TAG_IMPORT_INFO 1024

// max count: MAXIMUM_FILES_PER_TAG_IMPORT_INFO
struct tag_import_file
{
    char path[256];
    char modification_date[32];
    int8 pad[8];
    int8 pad1[88];
    int32 checksum_crc32;
    int32 size_bytes;

    data_block zipped_data; // Data size: 134217728
    int8 pad2[128];
};
TAG_BLOCK_SIZE_ASSERT(tag_import_file, 0x210);

struct tag_import_info
{
    int32 build;
    char version[256];
    char import_date[32];
    char culprit[32];
    int8 pad[96];
    char import_time[32];
    int32 pad1;
    tag_block<tag_import_file> files;
    int8 pad2[128];
};
TAG_BLOCK_SIZE_ASSERT(tag_import_info, 592);
