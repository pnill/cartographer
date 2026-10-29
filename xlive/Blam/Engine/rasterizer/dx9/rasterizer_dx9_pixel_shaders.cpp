#include "stdafx.h"
#include "rasterizer_dx9_pixel_shaders.h"

#include "cache/cache_files.h"
#include "shaders/shader_postprocess_definitions.h"
#include "shaders/shader_passes.h"

/* constants */

// Implementation 1 of these passes is the per-vertex lightmap implementation (objects lit through vertex colours: palms, bushes, scenery).
// Its pixel shaders declare texture stage 4 as a cube sampler, while every shader template using these passes binds a 2d bitmap there
// (shaders\default_bitmaps\bitmaps\alpha_white); implementations 0, 2 and 3 declare the same stage as 2d.
// Sampling a 2d texture through a cube sampler is undefined in Direct3D 9: older GPUs return the texel, Turing and newer return 0,
// so the alpha test discards every pixel and the foliage is invisible.
static const char* const k_shader_passes_with_cube_sampler_mismatch[] =
{
	"shaders\\shader_passes\\lightmap\\lightmap_tex_detail_alpha_test",
	"shaders\\shader_passes\\lightmap\\lightmap_tex_detail_alpha_test_clamped"
};

#define k_per_vertex_lightmap_implementation_index 1
#define k_mismatched_sampler_stage 4

/* prototypes */

static bool shader_pass_has_cube_sampler_mismatch(datum shader_pass_tag_index);

static void shader_pass_implementation_redeclare_sampler_as_2d(s_shader_pass_postprocess_implementation_new* implementation, uint32 sampler_stage);

static DWORD* pixel_shader_bytecode_find_sampler_declaration(DWORD* bytecode, uint32 token_count, uint32 sampler_stage);

/* public code */

void rasterizer_dx9_pixel_shaders_fix_map_bytecode(
	datum shader_pass_tag_index)
{
	if (!shader_pass_has_cube_sampler_mismatch(shader_pass_tag_index))
	{
		return;
	}

	s_shader_pass_definition* shader_pass = shader_pass_definition_get(shader_pass_tag_index);
	if (shader_pass->postprocess_definition.count == 0)
	{
		return;
	}

	s_shader_pass_postprocess_definition_new* postprocess = shader_pass->postprocess_definition[0];
	if (postprocess->implementations.count <= k_per_vertex_lightmap_implementation_index)
	{
		return;
	}

	s_shader_pass_postprocess_implementation_new* implementation = postprocess->implementations[k_per_vertex_lightmap_implementation_index];
	shader_pass_implementation_redeclare_sampler_as_2d(implementation, k_mismatched_sampler_stage);
	return;
}

/* private code */

static bool shader_pass_has_cube_sampler_mismatch(
	datum shader_pass_tag_index)
{
	const char* tag_name = tag_get_name(shader_pass_tag_index);
	for (uint32 i = 0; i < NUMBEROF(k_shader_passes_with_cube_sampler_mismatch); ++i)
	{
		if (csstricmp(tag_name, k_shader_passes_with_cube_sampler_mismatch[i]) == 0)
		{
			return true;
		}
	}
	return false;
}

// Changes the texture type of the sampler declaration from cube to 2d in every compiled variant (ps_2_0, ps_2_0 z-prime, ps_3_0)
// The plain texld that reads the sampler then uses the xy of the coordinate, which the shader already declares
// Runs before the game creates the Direct3D pixel shaders from this data (scenario_tags_postprocess, inside map_cache_load)
static void shader_pass_implementation_redeclare_sampler_as_2d(
	s_shader_pass_postprocess_implementation_new* implementation,
	uint32 sampler_stage)
{
	for (int32 pixel_shader_index = 0; pixel_shader_index < implementation->pixel_shader.count; ++pixel_shader_index)
	{
		s_pixel_shader* pixel_shader = implementation->pixel_shader[pixel_shader_index];
		for (uint32 variant = 0; variant < NUMBEROF(pixel_shader->compiled_shader); ++variant)
		{
			tag_data* compiled_shader = &pixel_shader->compiled_shader[variant];
			DWORD* bytecode = (DWORD*)tag_data_get_address(compiled_shader);
			if (bytecode == NULL)
			{
				continue;
			}

			const uint32 token_count = (uint32)compiled_shader->size / sizeof(DWORD);
			DWORD* usage_token = pixel_shader_bytecode_find_sampler_declaration(bytecode, token_count, sampler_stage);
			if (usage_token != NULL && (*usage_token & D3DSP_TEXTURETYPE_MASK) == D3DSTT_CUBE)
			{
				*usage_token = (*usage_token & ~D3DSP_TEXTURETYPE_MASK) | D3DSTT_2D;
			}
		}
	}
	return;
}

// Walks a ps_2_0+ token stream and returns the usage token of the dcl instruction for the sampler register, or NULL
static DWORD* pixel_shader_bytecode_find_sampler_declaration(
	DWORD* bytecode,
	uint32 token_count,
	uint32 sampler_stage)
{
	// sampler declarations only exist from ps_2_0 on
	if (token_count < 2 || (bytecode[0] & 0xFFFF0000) != D3DPS_VERSION(0, 0) || D3DSHADER_VERSION_MAJOR(bytecode[0]) < 2)
	{
		return NULL;
	}

	uint32 token = 1;
	while (token < token_count && bytecode[token] != D3DSIO_END)
	{
		const DWORD instruction = bytecode[token];
		const DWORD opcode = instruction & D3DSI_OPCODE_MASK;
		const uint32 length = opcode == D3DSIO_COMMENT ?
			(instruction & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT :
			(instruction & D3DSI_INSTLENGTH_MASK) >> D3DSI_INSTLENGTH_SHIFT;

		if (opcode == D3DSIO_DCL && length == 2 && token + 2 < token_count)
		{
			const DWORD reg = bytecode[token + 2];
			const DWORD register_type = ((reg & D3DSP_REGTYPE_MASK) >> D3DSP_REGTYPE_SHIFT) | ((reg & D3DSP_REGTYPE_MASK2) >> D3DSP_REGTYPE_SHIFT2);
			if (register_type == D3DSPR_SAMPLER && (reg & D3DSP_REGNUM_MASK) == sampler_stage)
			{
				return &bytecode[token + 1];
			}
		}
		token += 1 + length;
	}
	return NULL;
}
