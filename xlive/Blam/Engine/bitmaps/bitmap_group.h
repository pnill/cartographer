#pragma once
#include "tag_files/data_reference.h"
#include "tag_files/tag_block.h"

enum e_bitmap_tag_type : int16
{
    _bitmap_tag_type_2d_textures = 0,
    _bitmap_tag_type_3d_textures = 1,
    _bitmap_tag_type_cube_maps = 2,
    _bitmap_tag_type_sprites = 3,
    _bitmap_tag_type_interface_bitmaps = 4
};

enum e_bitmap_format : short
{
    bitmap_format_compressed_with_color_key_transparency = 0,
    bitmap_format_compressed_with_explicit_alpha = 1,
    bitmap_format_compressed_with_interpolated_alpha = 2,
    bitmap_format_16_bit_color = 3,
    bitmap_format_32_bit_color = 4,
    bitmap_format_monochrome = 5
};

enum e_bitmap_usage : short
{
    bitmap_usage_alpha_blend = 0,
    bitmap_usage_default = 1,
    bitmap_usage_height_map = 2,
    bitmap_usage_detail_map = 3,
    bitmap_usage_light_map = 4,
    bitmap_usage_vector_map = 5,
    bitmap_usage_height_map_blue_255 = 6,
    bitmap_usage_embm = 7,                      // Environment mapped bump map
    bitmap_usage_height_map_a8l8 = 8,
    bitmap_usage_height_map_g8b8 = 9,
    bitmap_usage_height_map_g8b8_w_alpha = 10
};

enum e_bitmap_flags : short
{
    bitmap_flag_enable_diffusion_dithering = FLAG(0),
    bitmap_flag_disable_height_map_compression = FLAG(1),
    bitmap_flag_uniform_sprite_sequences = FLAG(2),
    bitmap_flag_filthy_sprite_bug_fix = FLAG(3),
    bitmap_flag_use_sharp_bump_filter = FLAG(4),
    bitmap_flag_wdp_compression = FLAG(5),                      // Labeled as UNUSED in vista tagdefs
    bitmap_flag_use_clamped_mirrored_bump_filter = FLAG(6),
    bitmap_flag_invert_detail_fade = FLAG(7),
    bitmap_flag_swap_xy_vector_components = FLAG(8),
    bitmap_flag_convert_from_signed = FLAG(9),
    bitmap_flag_convert_to_signed = FLAG(10),
    bitmap_flag_import_mipmap_chains = FLAG(11),
    bitmap_flag_intentionally_true_color = FLAG(12)
};

enum e_sprite_budget_size : short
{
    sprite_budget_size_32x32 = 0,
    sprite_budget_size_64x64 = 1,
    sprite_budget_size_128x128 = 2,
    sprite_budget_size_256x256 = 3,
    sprite_budget_size_512x512 = 4,
    sprite_budget_size_1024x1024 = 5
};

enum e_sprite_usage : short
{
    sprite_usage_blend_add_subtract_max = 0,
    sprite_usage_multiply_min = 1,
    sprite_usage_double_multiply = 2
};

enum e_force_format : short
{
    force_format_default = 0,
    force_format_force_g8b8 = 1,
    force_format_force_dxt1 = 2,
    force_format_force_dxt3 = 3,
    force_format_force_dxt5 = 4,
    force_format_force_alpha_luminance8 = 5,
    force_format_force_a4r4g4b4 = 6,
};

enum e_bitmap_type : int16
{
    _bitmap_type_2d = 0,
    _bitmap_type_3d = 1,
    _bitmap_type_cubemap = 2,
    k_bitmap_type_count
};

#define MAXIMUM_SPRITES_PER_SEQUENCE 64
struct bitmap_group_sprite
{
    short bitmapIndex;
    short pad0;
    DWORD pad1;
    float left;
    float right;
    float top;
    float bottom;
    real_point2d registrationPoint;
};
ASSERT_STRUCT_SIZE(bitmap_group_sprite, 32);

#define MAXIMUM_SEQUENCES_PER_BITMAP_GROUP 256
struct bitmap_group_sequence
{
    char name[32];
    short first_bitmap_index;
    short bitmap_count;
    DWORD pad[4];
    tag_block<bitmap_group_sprite> sprites;
};
ASSERT_STRUCT_SIZE(bitmap_group_sequence, 0x3C);

enum e_more_bitmap_data_flags : byte
{
    more_bitmap_data_flag_delete_from_cache_file = FLAG(0),
    more_bitmap_data_flag_bitmap_create_attempted = FLAG(1)
};

enum e_bitmap_data_flags : short
{
    bitmap_data_flag_power_of_two_dimensions = FLAG(0),
    bitmap_data_flag_compressed = FLAG(1),
    bitmap_data_flag_palettized = FLAG(2),
    bitmap_data_flag_swizzled = FLAG(3),
    bitmap_data_flag_linear = FLAG(4),
    bitmap_data_flag_v16u16 = FLAG(5),
    bitmap_data_flag_mip_map_debug_level = FLAG(6),
    bitmap_data_flag_prefer_stutter_prefer_low_detail = FLAG(7)
};

enum e_bitmap_data_format : int16
{
    bitmap_data_format_a8 = 0,
    bitmap_data_format_y8 = 1,
    bitmap_data_format_ay8 = 2,
    bitmap_data_format_a8y8 = 3,
    bitmap_data_format_unused1 = 4,
    bitmap_data_format_unused2 = 5,
    bitmap_data_format_r5g6b5 = 6,
    bitmap_data_format_unused3 = 7,
    bitmap_data_format_a1r5g5b5 = 8,
    bitmap_data_format_a4r4g4b4 = 9,
    bitmap_data_format_x8r8g8b8 = 10,
    bitmap_data_format_a8r8g8b8 = 11,
    bitmap_data_format_unused4 = 12,
    bitmap_data_format_unused5 = 13,
    bitmap_data_format_dxt1 = 14,
    bitmap_data_format_dxt3 = 15,
    bitmap_data_format_dxt5 = 16,
    bitmap_data_format_p8bump = 17,
    bitmap_data_format_p8 = 18,
    bitmap_data_format_argbfp32 = 19,
    bitmap_data_format_rgbfp32 = 20,
    bitmap_data_format_rgbfp16 = 21,
    bitmap_data_format_v8u8 = 22,
    bitmap_data_format_g8b8 = 23,
    k_bitmap_format_count
};

enum e_bitmap_cache_usage : byte
{
    bitmap_cache_usage_default = 0,
    bitmap_cache_usage_environment = 1,
    bitmap_cache_usage_environment_bump= 2,
    bitmap_cache_usage_environment_lightmap = 3,
    bitmap_cache_usage_scenery = 4,
    bitmap_cache_usage_scenery_bump = 5,
    bitmap_cache_usage_weapon = 6,
    bitmap_cache_usage_weapon_bump = 7,
    bitmap_cache_usage_vehicle = 8,
    bitmap_cache_usage_vehicle_bump = 9,
    bitmap_cache_usage_biped = 10,
    bitmap_cache_usage_biped_bump = 11,
    bitmap_cache_usage_object = 12,
    bitmap_cache_usage_object_bump = 13,
    bitmap_cache_usage_effect = 14,
    bitmap_cache_usage_hud = 15,
    bitmap_cache_usage_ui = 16,
    bitmap_cache_usage_sky = 17,
    bitmap_cache_usage_first_person = 18,
    bitmap_cache_usage_rasterizer = 19
};

#define MAXIMUM_BITMAPS_PER_BITMAP_GROUP 65536
struct bitmap_data
{
    char signature[4];
    int16 width_pixels;
    int16 height_pixels;
    uint8 depth_pixels;                       // Depth is 1 for 2D textures and cube maps.

    e_more_bitmap_data_flags more_flags;
    e_bitmap_type type;                     // Determines bitmap "geometry."
    e_bitmap_data_format format;            // Determines how pixels are represented internally.
    e_bitmap_data_flags flags;
    point2d registration_point;
    int16 mipmap_count;
#ifdef TAGS_BUILD
    int16 low_detail_mipmap_count;
#else
    uint8 lod_adjust;
    e_bitmap_cache_usage cache_usage;
#endif
    int32 pixels_offset;

    // The below fields are not referenced or used in a "TAG" build of the game
    // However they're just replaced with padding of the same size
    int32 lod1_offset;
    int32 lod2_offset;
    int32 lod3_offset;
    uint32 unk0[3];
    int32 lod1_size;
    int32 lod2_size;
    int32 lod3_size;
    uint32 unk1[3];
    datum parent_bitmap_tag;        // References the parent bitmap tag this bitmap data is located in
    IDirect3DTexture9* d3dtexture;
    uint8* base_address;
    int32 field_58;
    int32 field_5C;
    int32 low_detail_offset;
    int32 low_detail_size;
    e_bitmap_data_format low_detail_format;
    int16 low_detail_width;
    int16 low_detail_height;
    int16 low_detail_depth;
    int32* field_70;
};
ASSERT_STRUCT_SIZE(bitmap_data, 116);

enum e_color_subsampling : byte
{
    color_subsampling_400 = 0,
    color_subsampling_420 = 1,
    color_subsampling_422 = 2,
    color_subsampling_444 = 3,
};

struct bitmap_group
{
    /*[Explaination("Type", "Type controls bitmap geometry. All dimensions must be a power of 2 except for SPRITES and INTERFACE BITMAPS:

    * 2D TEXTURES : Ordinary 2D textures will be generated.
    * 3D TEXTURES : Volume textures will be generated from each sequence of 2D texture slices.
    * CUBE MAPS : Generated from each consecutive set of six 2D textures in each sequence, all faces of a cube map must be square and the same size.
    * SPRITES : Sprite texture pages will be generated.
    * INTERFACE BITMAPS : Similar to 2D TEXTURES but without mipmaps and without the power of 2 restriction.")]*/
    e_bitmap_tag_type type;

    /*[Explaination("Format", "Format controls how pixels will be stored internally:

    * COMPRESSED WITH COLOR-KEY TRANSPARENCY: DXT1 compression, using 4 bits/pixel. 4-x-4 blocks of pixels, are reduced to two colors and interpolated, alpha channel uses color-key transparency instead of alpha from the plate (all zero-alpha pixels also have zero-color).
    * COMPRESSED WITH EXPLICIT ALPHA: DXT2/3 compression, using 8 bits/pixel. Same as DXT1 without the color key transparency, alpha channel uses alpha from plate quantized down to 4 bits/pixel.
    * COMPRESSED WITH INTERPOLATED ALPHA: DXT4/5 compression, using 8 bits/pixel. Same as DXT2/3, except alpha is smoother. Better for smooth alpha gradients, but worse for noisy alpha.
    * 16-BIT COLOR: Uses 16 bits/pixel. Depending on the alpha channel, bitmaps are quantized to either r5g6b5 (no alpha), a1r5g5b5 (1-bit alpha), or a4r4g4b4 (>1-bit alpha).
    * 32-BIT COLOR: Uses 32 bits/pixel. Very high quality and can have alpha at no added cost. This format takes up the most memory, however. Bitmap formats are x8r8g8b8 and a8r8g8b.
    * MONOCHROME: Uses either 8 or 16 bits/pixel. Bitmap formats are a8 (alpha), y8 (intensity), ay8 (combined alpha intensity), and a8y8 (separate alpha intensity).")]*/
    e_bitmap_format format;

    /*[Explaination("Usage", "Usage controls how mipmaps are generated:

    * ALPHA BLEND: Pixels with zero alpha are ignored in mipmaps, to prevent bleeding the transparent color.
    * DEFAULT: Downsampling works normally, as in Photoshop.
    * HEIGHT MAP: The bitmap (normally grayscale) is a height map that gets converted to a bump map. Uses <bump height> below. Alpha is passed through unmodified.
    * DETAIL MAP: Mipmap color fades to gray, controlled by <detail fade factor> below. Alpha fades to white.
    * LIGHT MAP: Generates no mipmaps. Do not use!
    * VECTOR MAP: Used mostly for special effects; pixels are treated as xyz vectors and normalized after downsampling. Alpha is passed through unmodified.")]*/
    e_bitmap_usage usage;

    e_bitmap_flags flags;

    // Explaination("Postprocessing", "These properties control how mipmaps are postprocessed.")
    float detail_fade_factor;   // 0 means fade to gray by last mipmap; 1 means fade to gray by first mipmap.
    float sharpen_amount;       // Sharpens mipmap after downsampling.
    float bump_height;          // the apparent height of the bump map above the triangle onto which it is textured, in texture repeats (i.e., 1.0 would be as high as the texture is wide).

    // The below fields are unused (labeled and might be used in h1?)
    e_sprite_budget_size sprite_budget_size;
    short sprite_budget_count;

    // Explaination("Color Plate", "The original image file used to import the bitmap group.")
    short color_plate_width_pixels;
    short color_plate_height_pixels;

    // DataSize(1073741824)
    data_reference color_plate_data;        // compressed_color_plate_data
    // DataSize(1073741824)
    data_reference bitmap_pixel_data;       // processed_pixel_data

    // Explaination("Miscellaneous", "")
    float blur_filter_size;             // Blurs the bitmap before generating mipmaps.
    float alpha_bias;                   // Affects alpha mipmap generation.
    short mipmap_count;                 // 0 Defaults to all levels.

    e_sprite_usage sprite_usage;
    short sprite_spacing;

    // Explaination("...More Stuff", "EMPTY STRING")
    e_force_format force_format;
    tag_block<bitmap_group_sequence> sequences;
    tag_block<bitmap_data> bitmaps;

    // Explaination("WDP fields", "")
    char color_compression_quality;    // 1 means lossless, 127 means crappy
    char alpha_compression_quality;    // 1 means lossless, 127 means crappy
    char overlap;
    e_color_subsampling color_subsampling;
};
ASSERT_STRUCT_SIZE(bitmap_group, 80);

bitmap_data* __cdecl bitmap_group_get_bitmap_from_sequence(datum tag_index, int16 sequence_index, int16 sprite_index);
