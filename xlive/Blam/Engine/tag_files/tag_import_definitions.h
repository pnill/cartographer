#pragma once

/* constants */

enum
{
	MAXIMUM_FILES_PER_TAG_IMPORT_INFO = 1024,
};

/* structures */

// max count: MAXIMUM_FILES_PER_TAG_IMPORT_INFO
struct tag_import_file
{
	char path[k_maximum_filename_length];
	char modification_date[k_tag_string_length];
	int8 pad[8];
	int8 pad1[88];
	int32 checksum_crc32;
	int32 size_bytes;

	tag_data zipped_data; // Data size: 134217728
	int8 pad2[128];
};
ASSERT_STRUCT_SIZE(tag_import_file, 0x210);

struct tag_import_info
{
	int32 build;
	char version[k_maximum_filename_length];
	char import_date[k_tag_string_length];
	char culprit[k_tag_string_length];
	int8 pad[96];
	char import_time[k_tag_string_length];
	int32 pad1;
	s_tag_block files;	// tag_import_file
	int8 pad2[128];
};
ASSERT_STRUCT_SIZE(tag_import_info, 592);
