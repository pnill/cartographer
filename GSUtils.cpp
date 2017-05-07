#include "GSUtils.h"
#include <stdio.h>
#include <windows.h>
#include <Wincrypt.h>

void OverwriteAssembly(BYTE* srcAddr, BYTE* writeAssm, int lenAssm) {
	DWORD dwBack;
	VirtualProtect(srcAddr, lenAssm, PAGE_EXECUTE_READWRITE, &dwBack);

	for (int i = 0; i < lenAssm; i++)
		srcAddr[i] = writeAssm[i];

	VirtualProtect(srcAddr, lenAssm, dwBack, &dwBack);
}

void HexToByteArray(BYTE* byteArray, char* pointerHex) {
	char totext2[32];
	memset(totext2, '0', 1);
	char* totext3 = totext2 + 1;
	sprintf(totext3, "%s", pointerHex);
	int len = strlen(totext3);
	if (len % 2 == 1) {
		totext3 = totext2;
		len++;
	}
	for (int i = 0; i < len / 2; i++) {
		char str[3];
		snprintf(str, 3, "%s", totext3 + (i * 2));
		byteArray[i] = (int)strtol(str, NULL, 16);
	}
}

int FindLineStart(FILE* fp, int lineStrLen) {
	int fp_offset_orig = ftell(fp);
	for (int i = lineStrLen; i < 255; i++) {
		if (fp_offset_orig - i < 0) {
			fseek(fp, fp_offset_orig, SEEK_SET);
			return 0;
		}
		fseek(fp, fp_offset_orig - i, SEEK_SET);
		int c = 0;
		if ((c = fgetc(fp)) == '\r' || c == '\n') {
			fseek(fp, fp_offset_orig, SEEK_SET);
			return fp_offset_orig - i + 1;
		}
	}
	fseek(fp, fp_offset_orig, SEEK_SET);
	return fp_offset_orig - lineStrLen;
}

void GetVKeyCodeString(int vkey, char* rtnString, int strLen) {
	snprintf(rtnString, 5, "0x%x", vkey);
	char key_name[20];
	memset(key_name, 0, 1);
	if (vkey >= 0x70 && vkey <= 0x87) {
		int func_num = vkey - 0x70 + 1;
		snprintf(key_name, 20, "VK_F%d", func_num);
	}
	if (strlen(key_name) > 0) {
		snprintf(rtnString + strlen(rtnString), strLen - 5, " - %s", key_name);
	}
}

void PadCStringWithChar(char* strToPad, int toFullLength, char c) {
	for (int i = strlen(strToPad); i < toFullLength - 1; i++) {
		memset(strToPad + i, c, sizeof(char));
	}
	memset(strToPad + toFullLength - 1, 0, sizeof(char));
}

int ComputeFileMd5Hash(wchar_t* filepath, char* rtnMd5) {
	for (int i = 0; i < 33; i++) {
		rtnMd5[i] = 0;
	}

	const int BUFSIZE = 1024;
	const int MD5LEN = 16;
	DWORD dwStatus = 0;
	BOOL bResult = FALSE;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	HANDLE hFile = NULL;
	BYTE rgbFile[BUFSIZE];
	DWORD cbRead = 0;
	BYTE rgbHash[MD5LEN];
	DWORD cbHash = 0;
	CHAR rgbDigits[] = "0123456789abcdef";
	//LPCWSTR filename = L"xinput9_1_0.dll";
	// Logic to check usage goes here.

	hFile = CreateFile(filepath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return -1;
	}

	// Get handle to the crypto provider
	if (!CryptAcquireContext(&hProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT))
	{
		dwStatus = GetLastError();
		strcpy(rtnMd5, "CryptAcquireContext");
		CloseHandle(hFile);
		return dwStatus;
	}

	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
	{
		dwStatus = GetLastError();
		strcpy(rtnMd5, "CryptAcquireContext");
		CloseHandle(hFile);
		CryptReleaseContext(hProv, 0);
		return dwStatus;
	}

	while (bResult = ReadFile(hFile, rgbFile, BUFSIZE, &cbRead, NULL))
	{
		if (0 == cbRead)
		{
			break;
		}

		if (!CryptHashData(hHash, rgbFile, cbRead, 0))
		{
			dwStatus = GetLastError();
			strcpy(rtnMd5, "CryptHashData");
			CryptReleaseContext(hProv, 0);
			CryptDestroyHash(hHash);
			CloseHandle(hFile);
			return dwStatus;
		}
	}

	if (!bResult)
	{
		dwStatus = GetLastError();
		strcpy(rtnMd5, "ReadFile");
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		CloseHandle(hFile);
		return dwStatus;
	}

	cbHash = MD5LEN;
	if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
	{
		for (DWORD i = 0; i < cbHash; i++)
		{
			rtnMd5[i * 2] = rgbDigits[rgbHash[i] >> 4];
			rtnMd5[(i * 2) + 1] = rgbDigits[rgbHash[i] & 0xf];
		}
	}
	else
	{
		dwStatus = GetLastError();
		strcpy(rtnMd5, "CryptGetHashParam");
	}

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	CloseHandle(hFile);

	return dwStatus;
}

int GetWidePathFromFullWideFilename(wchar_t* filepath, wchar_t* rtnpath) {
	wchar_t* offset = wcsrchr(filepath, L'\\');
	wchar_t* off2 = wcsrchr(filepath, L'/');
	offset = offset == NULL ? off2 : ((off2 != NULL && offset < off2) ? off2 : offset);
	if (offset == NULL) {
		return -1;
	}
	swprintf(rtnpath, offset - filepath + 2, filepath);
	return 0;
}

LONG GetDWORDRegKey(HKEY hKey, wchar_t* strValueName, DWORD* nValue) {
	DWORD dwBufferSize(sizeof(DWORD));
	DWORD nResult(0);
	LONG nError = ::RegQueryValueExW(hKey,
		strValueName,
		0,
		NULL,
		reinterpret_cast<LPBYTE>(&nResult),
		&dwBufferSize);
	if (ERROR_SUCCESS == nError)
	{
		*nValue = nResult;
	}
	return nError;
}
