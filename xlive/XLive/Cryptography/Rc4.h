#pragma once

/* Rc4: Credits to Xenia Xbox 360 emulator developers */

typedef struct {
	uint8_t S[256];  // 0x0
	uint8_t i;       // 0x100
	uint8_t j;       // 0x101
} XECRYPT_RC4_STATE;

void XeCryptRc4Ecb(XECRYPT_RC4_STATE* rc4_ctx, BYTE* data, DWORD size);
void XeCryptRc4Key(XECRYPT_RC4_STATE* rc4_ctx, const BYTE* key, DWORD key_size);