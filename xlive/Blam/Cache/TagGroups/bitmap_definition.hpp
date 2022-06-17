#include "stdafx.h"
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Cache\TagGroups.hpp"
#include "Blam\Math\BlamMath.h"

#pragma pack(push,1)

struct bitmap_definition
{
    /*[Explaination("Type", "Type controls bitmap geometry. All dimensions must be a power of 2 except for SPRITES and INTERFACE BITMAPS:

    * 2D TEXTURES : Ordinary 2D textures will be generated.
    * 3D TEXTURES : Volume textures will be generated from each sequence of 2D texture slices.
    * CUBE MAPS : Generated from each consecutive set of six 2D textures in each sequence, all faces of a cube map must be square and the same size.
    * SPRITES : Sprite texture pages will be generated.
    * INTERFACE BITMAPS : Similar to 2D TEXTURES but without mipmaps and without the power of 2 restriction.")]*/

    short bitmaptype;

    /*[Explaination("Format", "Format controls how pixels will be stored internally:
    
    * COMPRESSED WITH COLOR-KEY TRANSPARENCY: DXT1 compression, using 4 bits/pixel. 4-x-4 blocks of pixels, are reduced to two colors and interpolated, alpha channel uses color-key transparency instead of alpha from the plate (all zero-alpha pixels also have zero-color).
    * COMPRESSED WITH EXPLICIT ALPHA: DXT2/3 compression, using 8 bits/pixel. Same as DXT1 without the color key transparency, alpha channel uses alpha from plate quantized down to 4 bits/pixel.
    * COMPRESSED WITH INTERPOLATED ALPHA: DXT4/5 compression, using 8 bits/pixel. Same as DXT2/3, except alpha is smoother. Better for smooth alpha gradients, but worse for noisy alpha.
    * 16-BIT COLOR: Uses 16 bits/pixel. Depending on the alpha channel, bitmaps are quantized to either r5g6b5 (no alpha), a1r5g5b5 (1-bit alpha), or a4r4g4b4 (>1-bit alpha).
    * 32-BIT COLOR: Uses 32 bits/pixel. Very high quality and can have alpha at no added cost. This format takes up the most memory, however. Bitmap formats are x8r8g8b8 and a8r8g8b.
    * MONOCHROME: Uses either 8 or 16 bits/pixel. Bitmap formats are a8 (alpha), y8 (intensity), ay8 (combined alpha intensity), and a8y8 (separate alpha intensity).")]*/

    enum Format : short
    {
        CompressedWithColorKeyTransparency = 0,
        CompressedWithExplicitAlpha = 1,
        CompressedWithInterpolatedAlpha = 2,
        _16BitColor = 3,
        _32BitColor = 4,
        Monochrome = 5,
    };
    Format format;

    /*[Explaination("Usage", "Usage controls how mipmaps are generated:
    
    * ALPHA BLEND: Pixels with zero alpha are ignored in mipmaps, to prevent bleeding the transparent color.
    * DEFAULT: Downsampling works normally, as in Photoshop.
    * HEIGHT MAP: The bitmap (normally grayscale) is a height map that gets converted to a bump map. Uses <bump height> below. Alpha is passed through unmodified.
    * DETAIL MAP: Mipmap color fades to gray, controlled by <detail fade factor> below. Alpha fades to white.
    * LIGHT MAP: Generates no mipmaps. Do not use!
    * VECTOR MAP: Used mostly for special effects; pixels are treated as xyz vectors and normalized after downsampling. Alpha is passed through unmodified.")]*/

    enum Usage : short
    {
        AlphaBlend = 0,
        Default = 1,
        HeightMap = 2,
        DetailMap = 3,
        LightMap = 4,
        VectorMap = 5,
        HeightMapBLUE255 = 6,
        Embm = 7,
        HeightMapA8L8 = 8,
        HeightMapG8B8 = 9,
        HeightMapG8B8WAlpha = 10,
    };
    Usage usage;

    enum Flags : short
    {
        EnableDiffusionDithering = 0x1,
        DisableHeightMapCompression = 0x2,
        UniformSpriteSequences = 0x4,
        FilthySpriteBugFix = 0x8,
        UseSharpBumpFilter = 0x10,
        UNUSED = 0x20,
        UseClampedMirroredBumpFilter = 0x40,
        InvertDetailFade = 0x80,
        SwapXyVectorComponents = 0x100,
        ConvertFromSigned = 0x200,
        ConvertToSigned = 0x400,
        ImportMipmapChains = 0x800,
        IntentionallyTrueColor = 0x1000,
    };
    Flags flags;

    //[Explaination("Postprocessing", "These properties control how mipmaps are postprocessed.")]
    /// <summary>
    /// 0 means fade to gray by last mipmap; 1 means fade to gray by first mipmap.
    /// </summary>
    float detailFadeFactor;
    /// <summary>
    /// Sharpens mipmap after downsampling.
    /// </summary>
    float sharpenAmount;
    /// <summary>
    /// tApparent height of the bump map above the triangle onto which it is textured, in texture repeats (i.e., 1.0 would be as high as the texture is wide).
    /// </summary>
    float bumpHeightRepeats;

    enum EMPTYSTRING : short
    {
        _32X32 = 0,
        _64X64 = 1,
        _128X128 = 2,
        _256X256 = 3,
        _512X512 = 4,
        _1024X1024 = 5,
    };
    EMPTYSTRING SpriteBudgetSize;
    short SpriteBudgetCount;

    //[Explaination("Color Plate", "The original image file used to import the bitmap group.")]
    short colorPlateWidthPixels;
    short colorPlateHeightPixels;

    /****************************************
    * definition_name: color_plate_data
    * flags: 1
    * alignment_bit: 0
    ****************************************/
    //[DataSize(1073741824)]
    __int8 compressedColorPlateData;
    //[Explaination("Processed Pixel Data", "Pixel data after being processed by the tool.")]
    /****************************************
    * definition_name: bitmap_pixel_data
    * flags: 0
    * alignment_bit: 0
    ****************************************/
    //[DataSize(1073741824)]
    __int8 processedPixelData;

    PAD(14);

    //[Explaination("Miscellaneous", "EMPTY STRING")]
    /// <summary>
    /// Blurs the bitmap before generating mipmaps.
    /// </summary>
    float blurFilterSize010Pixels;
    /// <summary>
    /// Affects alpha mipmap generation.
    /// </summary>
    float alphaBias11;
    /// <summary>
    /// 0 Defaults to all levels.
    /// </summary>
    short mipmapCountLevels;
    //[Explaination("...More Sprite Processing", "Sprite usage controls the background color of sprite plates.")]

    enum SpriteUsage : short
    {
        BlendAddSubtractMax = 0,
        MultiplyMin = 1,
        DoubleMultiply = 2,
    };
    SpriteUsage spriteUsage;
    short spriteSpacing;
    //[Explaination("...More Stuff", "EMPTY STRING")]

    enum ForceFormat : short
    {
        _Default = 0,
        ForceG8B8 = 1,
        ForceDXT1 = 2,
        ForceDXT3 = 3,
        ForceDXT5 = 4,
        ForceALPHALUMINANCE8 = 5,
        ForceA4R4G4B4 = 6,
    };
    ForceFormat forceFormat;

    struct bitmap_group_sequence_block
    {
        char name[32];
        short firstBitmapIndex;
        short bitmapCount;
        PAD(16);
        struct bitmap_group_sprite_block
        {
            short bitmapIndex;
            PAD(2);
            PAD(4);
            float left;
            float right;
            float top;
            float bottom;
            real_vector2d registrationPoint;
        };
        CHECK_STRUCT_SIZE(bitmap_group_sprite_block, 32);
        tag_block<bitmap_group_sprite_block> sprites;
    };
    CHECK_STRUCT_SIZE(bitmap_group_sequence_block, 60);
    tag_block<bitmap_group_sequence_block> sequences;

    struct bitmap_data_block
    {
        char signature[4];
        point2d size;
        /// <summary>
        /// Depth is 1 for 2D textures and cube maps.
        /// </summary>
        __int8 depthPixels;

        enum MoreFlags : __int8
        {
            DeleteFromCacheFile = 0x1,
            BitmapCreateAttempted = 0x2,
        };
        MoreFlags moreFlags;

        enum Type : short
        {
            _2DTexture = 0,
            _3DTexture = 1,
            CubeMap = 2,
        };
        /// <summary>
        /// Determines bitmap "geometry."
        /// </summary>
        Type type;

        enum Format : short
        {
            A8 = 0,
            Y8 = 1,
            Ay8 = 2,
            A8y8 = 3,
            Unused1 = 4,
            Unused2 = 5,
            R5g6b5 = 6,
            Unused3 = 7,
            A1r5g5b5 = 8,
            A4r4g4b4 = 9,
            X8r8g8b8 = 10,
            A8r8g8b8 = 11,
            Unused4 = 12,
            Unused5 = 13,
            Dxt1 = 14,
            Dxt3 = 15,
            Dxt5 = 16,
            P8bump = 17,
            P8 = 18,
            Argbfp32 = 19,
            Rgbfp32 = 20,
            Rgbfp16 = 21,
            V8u8 = 22,
            G8b8 = 23,
        };
        /// <summary>
        /// Determines how pixels are represented internally.
        /// </summary>
        Format format;

        enum Flags : short
        {
            PowerOfTwoDimensions = 0x1,
            Compressed = 0x2,
            Palettized = 0x4,
            Swizzled = 0x8,
            Linear = 0x10,
            V16u16 = 0x20,
            MIPMapDebugLevel = 0x40,
            PreferStutterPreferLowDetail = 0x80,
        };
        Flags flags;
        real_vector2d registrationPoint;
        short mipmapCount;
        short lowDetailMipmapCount;
        int pixelsOffset;
        int lod_1_offset;
        int lod_2_offset;
        int lod_3_offset;
        PAD(4);
        PAD(4);
        PAD(4);
        int lod_1_size;
        int lod_2_size;
        int lod_3_size;
        PAD(4);
        PAD(4);
        PAD(4);
        tag_reference datum;
        PAD(4);
        PAD(4);
        PAD(4);
        PAD(4);
        PAD(4);
        PAD(4);
        PAD(4);
    };
    CHECK_STRUCT_SIZE(bitmap_data_block, 116);
    tag_block<bitmap_data_block> bitmaps;

    //[Explaination("WDP fields", "")]
    /// <summary>
    /// 1 means lossless, 127 means crappy
    /// </summary>
    __int8 colorCompressionQuality1127;
    /// <summary>
    /// 1 means lossless, 127 means crappy
    /// </summary>
    __int8 alphaCompressionQuality1127;
    __int8 overlap;

    enum ColorSubsampling : __int8
    {
        _400 = 0,
        _420 = 1,
        _422 = 2,
        _444 = 3,
    };
    ColorSubsampling colorSubsampling;
};
constexpr size_t sizeOfT = sizeof(bitmap_definition);

CHECK_STRUCT_SIZE(bitmap_definition, 80);
#pragma pack(pop)