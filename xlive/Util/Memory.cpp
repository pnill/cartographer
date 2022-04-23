#include "stdafx.h"

#include "Memory.h"

DWORD Memory::baseAddress;
bool Memory::dedicatedServer;

ProcessInfo game_info;

H2Type DetectProcessType()
{
	// try and detect type based on module name.
	if (GetModuleHandleW(L"halo2.exe"))
		return H2Type::H2Game;
	else if (GetModuleHandleW(L"h2server.exe"))
		return H2Type::H2Server;

	// fallback to checking file information in case the file was renamed.
	wchar_t exe_file_path[_MAX_PATH + 1];
	int result = GetModuleFileNameW(NULL, exe_file_path, ARRAYSIZE(exe_file_path));
	if (result <= _MAX_PATH && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		DWORD version_info_size = GetFileVersionInfoSizeW(exe_file_path, NULL);
		if (version_info_size != 0) {

			BYTE* version_info = new BYTE[version_info_size];
			if (GetFileVersionInfoW(exe_file_path, NULL, version_info_size, version_info)) {
				wchar_t* orginal_filename;
				size_t filename_len;
				// shouldn't be hardcoded but who cares
				VerQueryValueW(version_info, L"\\StringFileInfo\\040904b0\\OriginalFilename", (LPVOID*)&orginal_filename, &filename_len);

				std::wstring exe_orginal_filename = orginal_filename;
				delete[] version_info;

				if (exe_orginal_filename == L"halo2.exe")
					return H2Type::H2Game;
				else if (exe_orginal_filename == L"h2server.exe")
					return H2Type::H2Server;
			}
		}
	}
	return H2Type::Invalid;
}

// TODO maybe this is not the best place to keep this
void Memory::Initialize()
{
	game_info.base = GetModuleHandle(NULL);
	game_info.process_type = DetectProcessType();

	switch (game_info.process_type)
	{
	case H2Type::Invalid:
		MessageBoxA(NULL, "Project Cartographer loaded into unsupported process, will now exit!", "ERROR!", MB_OK);
		TerminateProcess(GetCurrentProcess(), 1);
		break;
	case H2Type::H2Game:
		Memory::SetBaseAddress((DWORD)game_info.base, false);
		break;
	case H2Type::H2Server:
		Memory::SetBaseAddress((DWORD)game_info.base, true);
		break;
	default:
		assert(0);
		break;
	}
}

// use this only if input is expected to always be properly formated
// preper format means it only contains only these characters: '01234556789ABCDEFabcdef' and no (pre/su)fixes
void HexStrToBytesUnsafe(const char* hexStr, size_t hexStrLen, BYTE* byteBuf, size_t bufLen)
{
	// ASCII character index map to hex value
	static const BYTE lutStrToHex[] = {
		0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  0,  0,  0,  0,
		0,  0,  0,  10,  11,  12,  13,  14,  15,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  10,  11,  12,  13,  14,  15
	};

	const BYTE* byteBufEnd = byteBuf + bufLen;
	hexStrLen = hexStrLen != 0u ? hexStrLen : strlen(hexStr);
	// continue only if string is at least 1
	if (hexStrLen >= 1u)
	{
		bool hexStrOddLen = !!(hexStrLen % 2);
		const char tmpBuf1[3] = { '0', hexStr[0], '\0' };
		const char* HexStrBeg = hexStrOddLen ? tmpBuf1 : &hexStr[0];

		for (int strIdx = 0; strIdx < hexStrLen && byteBuf < byteBufEnd; )
		{
			*byteBuf++ = (BYTE)((lutStrToHex[HexStrBeg[strIdx++] - '0'] << 4) | (lutStrToHex[HexStrBeg[strIdx++] - '0']));
			HexStrBeg = hexStr; // get the next byte
		}
	}
}

// TODO: this function checks the input hexStr before converting it to bytes
// hence being slower if we don't trust the input
void HexStrToBytes(const std::string& hexStr, BYTE* byteBuf, size_t bufLen) {
	size_t hexStrLen = hexStr.length();
	HexStrToBytesUnsafe(hexStr.c_str(), hexStrLen, byteBuf, bufLen);
}

std::string ByteToHexStr(const BYTE* buffer, size_t size) {
	std::stringstream str;
	str.setf(std::ios_base::hex, std::ios::basefield);
	str.setf(std::ios_base::uppercase);
	str.fill('0');

	for (size_t i = 0; i < size; i++) {
		str << std::setw(2) << (unsigned short)(BYTE)buffer[i];
	}
	return str.str();
}
