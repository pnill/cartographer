#include "stdafx.h"
#include "rasterizer_dx9_bytecode_fixes.h"

#include "networking/network_event.h"

#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

/* constants */

// 'dcl' usage token of a sampler register: (1 << 31) | (texture type << 27)
#define k_dcl_usage_token_cube_sampler 0x98000000
#define k_dcl_usage_token_2d_sampler 0x90000000

// IDirect3DDevice9::CreatePixelShader
#define k_d3d9_device_vtable_create_pixel_shader_index 106

// no stock pixel shader is larger than this (the tag data field is limited to 65535 bytes)
#define k_maximum_bytecode_token_count 16384

#define k_sha256_hex_length 64

/* structures */

struct s_pixel_shader_bytecode_fix
{
	uint32 size_bytes;			// stream length in bytes, including the END token
	const char* sha256_hex;		// SHA-256 of the original stream
	uint32 dword_index;			// token to rewrite
};

/* globals */

// dcl_cube s4 -> dcl_2d s4 in implementation 1 (per-vertex lightmap) of the passes
//   shaders\shader_passes\lightmap\lightmap_tex_detail_alpha_test
//   shaders\shader_passes\lightmap\lightmap_tex_detail_alpha_test_clamped
// The 9 distinct streams of all stock maps (ps_2_0, ps_2_0 z-prime and ps_3_0 MRT variants of the 4 elements).
// Each patch replaces exactly one dword, 0x98000000 -> 0x90000000, the stream size does not change.
static const s_pixel_shader_bytecode_fix k_pixel_shader_bytecode_fixes[] =
{
	{  820, "abf5a850c97f4382fdf12b3d951ccb809d4a7e91c65c80a65429c677118edaeb",  99 },
	{  700, "4a2d47feea0ca6b014fd1c42e0845779f9aeef86388d80b95427120678407e1d",  82 },
	{ 1012, "84d14d17f1b8c3caf3b04f868294069c7d77c32c6a1a407f45b0a6991a3946cb", 121 },
	{ 1236, "7dfe85fa3949a7c948bd0eb36a16e7998dbc7f2e87fad1731c67e44ca3596f5d", 118 },
	{ 1396, "7841357bf391560d84b327a6df679059dc87603e9b3d557f0b93ee6a8fbf7aae", 135 },
	{ 1204, "7de3b0b47a8024c7046162a743439fa92a52f685f6f7b4646fc74b7f640952dc", 118 },
	{ 1364, "ae70d437a9d07631b2f4e84ae37f73603985ff66ba40d248493b18a9e2eb2fd7", 135 },
	{ 1256, "8e2d07ff4c853f1e0dec92b38d373b94973c19a76583b261e4fdd4f255edd42b", 118 },
	{ 1416, "314238bcce30f9db852d219aff91f0fbea66505119fd6cbe33c39a578ebed4fb", 135 },
};

typedef HRESULT(STDMETHODCALLTYPE* create_pixel_shader_t)(IDirect3DDevice9* device, const DWORD* function, IDirect3DPixelShader9** shader);
static create_pixel_shader_t p_create_pixel_shader;

static bool g_pixel_shader_bytecode_fix_logged[NUMBEROF(k_pixel_shader_bytecode_fixes)];
static bool g_bytecode_fixes_installed = false;

/* prototypes */

static uint32 d3d9_pixel_shader_bytecode_size(const uint32* tokens);
static bool d3d9_sha256_hex(const void* data, uint32 size, char out_hex[k_sha256_hex_length + 1]);
static bool pixel_shader_bytecode_try_fix(const uint32* tokens, uint32 size, std::vector<uint32>& out_patched);
static HRESULT STDMETHODCALLTYPE create_pixel_shader_hook(IDirect3DDevice9* device, const DWORD* function, IDirect3DPixelShader9** shader);

/* public code */

void rasterizer_dx9_bytecode_fixes_initialize(IDirect3DDevice9* device)
{
	if (g_bytecode_fixes_installed || device == NULL)
	{
		return;
	}

	void** vtable = *reinterpret_cast<void***>(device);

	DETOUR_BEGIN();
	DETOUR_ATTACH(
		p_create_pixel_shader,
		reinterpret_cast<create_pixel_shader_t>(vtable[k_d3d9_device_vtable_create_pixel_shader_index]),
		create_pixel_shader_hook);
	DETOUR_COMMIT();

	g_bytecode_fixes_installed = true;
	return;
}

/* private code */

// Returns the size in bytes of a ps_2_x / ps_3_0 byte code stream (END token included),
// or 0 if the stream is of another type (ps_1_x tokens carry no length) or malformed
static uint32 d3d9_pixel_shader_bytecode_size(const uint32* tokens)
{
	uint32 result = 0;

	__try
	{
		const uint32 version = tokens[0];
		const uint32 shader_model_major = (version >> 8) & 0xFF;
		if ((version >> 16) == 0xFFFF && (shader_model_major == 2 || shader_model_major == 3))
		{
			for (uint32 i = 1; i < k_maximum_bytecode_token_count;)
			{
				const uint32 token = tokens[i];
				if (token == 0x0000FFFF)
				{
					// end token
					result = (i + 1) * sizeof(uint32);
					break;
				}

				if ((token & 0xFFFF) == 0xFFFE)
				{
					// comment block (constant table), the length is in bits 16..30
					i += 1 + ((token >> 16) & 0x7FFF);
				}
				else
				{
					// instruction, the operand count is in bits 24..27
					i += 1 + ((token >> 24) & 0x0F);
				}
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		result = 0;
	}

	return result;
}

static bool d3d9_sha256_hex(const void* data, uint32 size, char out_hex[k_sha256_hex_length + 1])
{
	bool result = false;

	BCRYPT_ALG_HANDLE algorithm = NULL;
	BCRYPT_HASH_HANDLE hash = NULL;
	uint8 digest[32];

	if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, NULL, 0) >= 0)
	{
		if (BCryptCreateHash(algorithm, &hash, NULL, 0, NULL, 0, 0) >= 0)
		{
			if (BCryptHashData(hash, (PUCHAR)data, size, 0) >= 0 &&
				BCryptFinishHash(hash, digest, sizeof(digest), 0) >= 0)
			{
				static const char k_hex_digits[] = "0123456789abcdef";
				for (uint32 i = 0; i < sizeof(digest); i++)
				{
					out_hex[i * 2] = k_hex_digits[digest[i] >> 4];
					out_hex[i * 2 + 1] = k_hex_digits[digest[i] & 0x0F];
				}
				out_hex[k_sha256_hex_length] = '\0';
				result = true;
			}
			BCryptDestroyHash(hash);
		}
		BCryptCloseAlgorithmProvider(algorithm, 0);
	}

	return result;
}

// Returns true and the patched copy of the stream when it is one of the known stock streams
static bool pixel_shader_bytecode_try_fix(const uint32* tokens, uint32 size, std::vector<uint32>& out_patched)
{
	bool result = false;

	char hex[k_sha256_hex_length + 1];
	bool hashed = false;

	for (uint32 i = 0; i < NUMBEROF(k_pixel_shader_bytecode_fixes); i++)
	{
		const s_pixel_shader_bytecode_fix* fix = &k_pixel_shader_bytecode_fixes[i];
		if (fix->size_bytes != size)
		{
			continue;
		}

		// only streams of a matching size are hashed
		if (!hashed)
		{
			if (!d3d9_sha256_hex(tokens, size, hex))
			{
				break;
			}
			hashed = true;
		}

		if (strcmp(hex, fix->sha256_hex) != 0)
		{
			continue;
		}

		// never patch a stream that is not exactly what we verified
		if (tokens[fix->dword_index] != k_dcl_usage_token_cube_sampler)
		{
			break;
		}

		out_patched.assign(tokens, tokens + size / sizeof(uint32));
		out_patched[fix->dword_index] = k_dcl_usage_token_2d_sampler;

		if (!g_pixel_shader_bytecode_fix_logged[i])
		{
			g_pixel_shader_bytecode_fix_logged[i] = true;
			event(_event_status, "rasterizer:dx9: pixel shader %s: sampler s4 declared as 2d instead of cube", fix->sha256_hex);
		}

		result = true;
		break;
	}

	return result;
}

static HRESULT STDMETHODCALLTYPE create_pixel_shader_hook(IDirect3DDevice9* device, const DWORD* function, IDirect3DPixelShader9** shader)
{
	std::vector<uint32> patched;
	const DWORD* bytecode = function;

	if (function != NULL)
	{
		const uint32 size = d3d9_pixel_shader_bytecode_size(reinterpret_cast<const uint32*>(function));
		if (size != 0 && pixel_shader_bytecode_try_fix(reinterpret_cast<const uint32*>(function), size, patched))
		{
			// d3d copies the byte code while creating the shader, the tag data stays untouched
			bytecode = reinterpret_cast<const DWORD*>(patched.data());
		}
	}

	return p_create_pixel_shader(device, bytecode, shader);
}
