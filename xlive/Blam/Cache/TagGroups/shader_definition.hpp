#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Cache\TagGroups.hpp"

#pragma pack(push,1)

struct shader_postprocess_definition_new_block
{
	DWORD shaderTemplateIndex;

	struct shader_postprocess_bitmap_new_block
	{
		tag_reference bitmapGroup;
		float logBitmapDimension;
	};
	CHECK_STRUCT_SIZE(shader_postprocess_bitmap_new_block, 12);
	tag_block<shader_postprocess_bitmap_new_block> bitmaps;

	tag_block<> pixelConstants;

	tag_block<> vertexConstants;

	tag_block<> levelsOfDetail;

	tag_block<> layers;

	tag_block<> passes;

	tag_block<> implementations;

	tag_block<> overlays;

	tag_block<> overlayReferences;

	tag_block<> animatedParameters;

	tag_block<> animatedParameterReferences;

	tag_block<> bitmapProperties;

	tag_block<> colorProperties;

	tag_block<> valueProperties;

	tag_block<> oldLevelsOfDetail;
};
//CHECK_STRUCT_SIZE(shader_definition, 184);

/*********************************************************************
* name: shader_block
* group_tag: shad
**********************************************************************/

struct shader_definition : TagGroup<'shad'>
{
	// TagReference("stem")
	tag_reference shader_template;
	DWORD materialName;
	tag_block<> runtimeProperties;

	PAD(2);

	enum Flags : short
	{
		Water = 0x1,
		SortFirst = 0x2,
		NoActiveCamo = 0x4,
	};
	Flags flags;
	tag_block<> parameters;

	tag_block<shader_postprocess_definition_new_block> postprocessDefinition;

	PAD(4);
	tag_block<> predictedResources;

	// TagReference("slit")
	tag_reference lightResponse;

	enum ShaderLODBias : short
	{
		_None = 0,
		_4xSize = 1,
		_2xSize = 2,
		_12Size = 3,
		_14Size = 4,
		_Never = 5,
		_Cinematic = 6,
	};
	ShaderLODBias shaderLODBias;

	enum SpecularType : short
	{
		None = 0,
		Default = 1,
		Dull = 2,
		Shiny = 3,
	};
	SpecularType specularType;

	enum LightmapType : short
	{
		Diffuse = 0,
		DefaultSpecular = 1,
		DullSpecular = 2,
		ShinySpecular = 3,
	};
	LightmapType lightmapType;
	PAD(2);
	float lightmapSpecularBrightness;
	float lightmapAmbientBias11;
	tag_block<> postprocessProperties;

	float addedDepthBiasOffset;
	float addedDepthBiasSlopeScale;
};
CHECK_STRUCT_SIZE(shader_definition, 92);
#pragma pack(pop)
