#include "Rc4.h"

void XeCryptRc4Key(XECRYPT_RC4_STATE* rc4_ctx, BYTE* key, DWORD key_size)
{
	// Setup RC4 state
	rc4_ctx->i = rc4_ctx->j = 0;
	for (uint32_t x = 0; x < 0x100; x++) {
		rc4_ctx->S[x] = (uint8_t)x;
	}

	uint32_t idx = 0;
	for (uint32_t x = 0; x < 0x100; x++) {
		idx = (idx + rc4_ctx->S[x] + key[x % key_size]) % 0x100;
		uint8_t temp = rc4_ctx->S[idx];
		rc4_ctx->S[idx] = rc4_ctx->S[x];
		rc4_ctx->S[x] = temp;
	}
}

void XeCryptRc4Ecb(XECRYPT_RC4_STATE* rc4_ctx, BYTE* data, DWORD size) {
	// Crypt data
	for (uint32_t idx = 0; idx < size; idx++) {
		rc4_ctx->i = (rc4_ctx->i + 1) % 0x100;
		rc4_ctx->j = (rc4_ctx->j + rc4_ctx->S[rc4_ctx->i]) % 0x100;
		uint8_t temp = rc4_ctx->S[rc4_ctx->i];
		rc4_ctx->S[rc4_ctx->i] = rc4_ctx->S[rc4_ctx->j];
		rc4_ctx->S[rc4_ctx->j] = temp;

		uint8_t a = data[idx];
		uint8_t b =
			rc4_ctx->S[(rc4_ctx->S[rc4_ctx->i] + rc4_ctx->S[rc4_ctx->j]) % 0x100];
		data[idx] = (uint8_t)(a ^ b);
	}
}