#include "GSUtils.h"
#include "H2OnscreenDebugLog.h"
#include "Hook.h"
#include <stdio.h>
#include <windows.h>
#include <Wincrypt.h>
#include "H2Config.h"

#include "libcurl/curl/curl.h"

#ifdef _DEBUG
#pragma comment(lib, "libcurl_a_debug.lib")
#else
#pragma comment(lib, "libcurl_a.lib")
#endif

void PatchCall(DWORD call_addr, DWORD new_function_ptr) {
	DWORD callRelative = new_function_ptr - (call_addr + 5);
	WritePointer(call_addr + 1, reinterpret_cast<void*>(callRelative));
}

void WritePointer(DWORD offset, void *ptr) {
	BYTE* pbyte = (BYTE*)&ptr;
	BYTE assmNewFuncRel[0x4] = { pbyte[0], pbyte[1], pbyte[2], pbyte[3] };
	WriteBytesASM(offset, assmNewFuncRel, 0x4);
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

bool GetFileLine(FILE* fp, char* &fileLine) {
	bool moretogo = true;
	int fileLineLen = 256;
	fileLine = (char*)malloc(fileLineLen);
	int fileLineCursor = 0;
	int c;
	while (c = fgetc(fp)) {
		if (c == EOF) {
			moretogo = false;
			break;
		}
		else if (c == '\r') {

		}
		else if (c == '\n') {
			break;
		}
		else {
			fileLine[fileLineCursor++] = c;
		}
		if (fileLineCursor >= fileLineLen - 2) {
			fileLineLen += 256;
			fileLine = (char*)realloc(fileLine, fileLineLen);
		}
	}
	fileLine[fileLineCursor++] = 0;
	if (strlen(fileLine) == 0) {
		free(fileLine);
		fileLine = 0;
	}
	return moretogo || fileLine != 0;
}

void ReadIniFile(void* fileConfig, bool configIsFILE, const char* header, const int headerVersion, int(interpretSettingFunc)(char* fileLine, int version, int lineNumber)) {
	bool foundFirstHeader = false;
	int version = -1;
	bool keepReading = true;
	int lineNumber = 0;
	char* fileLine;
	while (keepReading && ((configIsFILE && GetFileLine((FILE*)fileConfig, fileLine)) || (!configIsFILE && false))) {//TODO
		lineNumber++;
		if (fileLine) {
			if (fileLine[0] == header[0] && sscanf(fileLine, header, &version)) {
				foundFirstHeader = true;
				char debugTextBuffer[50];
				snprintf(debugTextBuffer, 50, "Found header on line %d asseting version: %d", lineNumber, version);
				addDebugText(debugTextBuffer);
				if (version == headerVersion) {
					free(fileLine);
					continue;
				}
				else {
					addDebugText("Incorrect Version! Continue searching!");
				}
			}
			int rtnCode = 0;
			if (foundFirstHeader) {
				rtnCode = interpretSettingFunc(fileLine, version, lineNumber);
			}
			if (!(rtnCode & 0b10)) {
				free(fileLine);
				if (rtnCode & 0b1)
					keepReading = false;
			}
		}
	}
}

void GetVKeyCodeString(int vkey, char* rtnString, int strLen) {
	snprintf(rtnString, 5, "0x%x", vkey);
	char key_name[20];
	memset(key_name, 0, 1);
	if (vkey >= 0x70 && vkey <= 0x87) {
		int func_num = vkey - 0x70 + 1;
		snprintf(key_name, 20, "VK_F%d", func_num);
	}
	else if (vkey == 0x24) {
		snprintf(key_name, 20, "VK_Home");
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

void pushHostLobby() {
	char msg[100] = { 0x00, 0x43, 0x05 };
	sprintf(msg + 3, "push clientlobby %d", H2Config_base_port + 1);

	addDebugText("Pushing open lobby.");

	int socketDescriptor;
	struct sockaddr_in serverAddress;
	if ((socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		addDebugText("ERROR: Could not create socket.");
	}
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = H2Config_master_ip;
	serverAddress.sin_port = htons(H2Config_master_port_relay);

	if (sendto(socketDescriptor, msg, strlen(msg + 3) + 3, 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
		//returns -1 if it wasn't successful. Note that it doesn't return -1 if the connection couldn't be established (UDP)
		addDebugText("ERROR: Failed to push open lobby.");
	}
}

char* custom_label_literal(char* label_escaped) {
	int label_escaped_length = strlen(label_escaped);
	int length_shift = 0;
	char* label_literal = (char*)malloc(label_escaped_length + 1);

	for (int i = 0; i < label_escaped_length; i++) {
		if (label_escaped[i] == '\\' && label_escaped[i + 1] == '\\') {
			label_literal[i + length_shift] = '\\';
			length_shift--;
			i++;
		}
		else if (label_escaped[i] == '\\' && label_escaped[i + 1] == 'n') {
			label_literal[i + length_shift] = '\n';
			length_shift--;
			i++;
		}
		else if (label_escaped[i] == '\\' && label_escaped[i + 1] == 'r') {
			label_literal[i + length_shift] = '\r';
			length_shift--;
			i++;
		}
		else {
			label_literal[i + length_shift] = label_escaped[i];
		}
	}

	label_literal[label_escaped_length + length_shift] = 0;
	return label_literal;
}

char* custom_label_escape(char* label_literal) {
	int label_literal_length = strlen(label_literal);
	int length_shift = 0;
	int label_escaped_length = label_literal_length + length_shift + 2;
	char* label_escaped = (char*)malloc(label_escaped_length);

	for (int i = 0; i < label_literal_length; i++) {
		if (label_escaped_length < i + length_shift + 2) {
			label_escaped = (char*)realloc(label_escaped, label_literal_length + length_shift + 2);
		}
		if (label_literal[i] == '\\') {
			label_escaped[i + length_shift] = '\\';
			length_shift++;
			label_escaped[i + length_shift] = '\\';
		}
		else if (label_literal[i] == '\n') {
			label_escaped[i + length_shift] = '\\';
			length_shift++;
			label_escaped[i + length_shift] = 'n';
		}
		else if (label_literal[i] == '\r') {
			label_escaped[i + length_shift] = '\\';
			length_shift++;
			label_escaped[i + length_shift] = 'r';
		}
		else {
			label_escaped[i + length_shift] = label_literal[i];
		}
	}

	label_escaped[label_literal_length + length_shift] = 0;
	return label_escaped;
}

bool FloatIsNaN(float &vagueFloat) {
	DWORD* vague = (DWORD*)&vagueFloat;
	if ((*vague >= 0x7F800000 && *vague <= 0x7FFFFFFF) || (*vague >= 0xFF800000 && *vague <= 0xFFFFFFFF)) {
		return true;
	}
	return false;
}

int HostnameToIp(char* hostname, char* ip) {
	struct hostent *he;
	struct in_addr **addr_list;
	int i;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if ((he = gethostbyname(hostname)) == NULL) {
		return WSAGetLastError();
	}

	addr_list = (struct in_addr **) he->h_addr_list;

	for (i = 0; addr_list[i] != NULL; i++)
	{
		//Return the first one;
		strncpy(ip, inet_ntoa(*addr_list[i]), 100);
		return 0;
	}

	return 1;
}

DWORD crc32buf(char* buf, size_t len)
{
	register DWORD oldcrc32;

	oldcrc32 = 0xFFFFFFFF;

	for (; len; --len, ++buf)
	{
		oldcrc32 = UPDC32(*buf, oldcrc32);
	}

	return ~oldcrc32;
}

static bool rfc3986_allow(char i) {
	//isalnum(i)//PoS crashes
	if ((i >= '0' && i <= '9') || 
		(i >= 'a' && i <= 'z') ||
		(i >= 'A' && i <= 'Z') ||
		i == '~' || i == '-' || i == '.' || i == '_')
		return true;
	return false;
}

char* escape_rfc3986(char* label_literal) {
	int label_literal_length = strlen(label_literal);
	int escaped_buflen = (label_literal_length * 3) + 1;
	char* label_escaped = (char*)malloc(escaped_buflen * sizeof(char));
	int escaped_buff_i = 0;

	for (int i = 0; i < label_literal_length; i++) {
		if (label_literal[i] < 0 || label_literal[i] > 0xFF) {//fuckin negative assholes.

		}
		else if (!rfc3986_allow(label_literal[i])) {
			sprintf_s(label_escaped + escaped_buff_i, 4, "%%%02X", label_literal[i]);
			escaped_buff_i += 2;
		}
		else {
			label_escaped[escaped_buff_i] = label_literal[i];
		}
		escaped_buff_i++;
	}
	label_escaped[escaped_buff_i] = 0;
	return label_escaped;
}

void wcstombs2(char* buffer, wchar_t* text, int buf_len) {
	int loop_len = wcslen(text);
	if (loop_len > buf_len)
		loop_len = buf_len;
	int buffer_i = 0;
	for (int i = 0; i < loop_len; i++) {
		if (text[i] >= 0 && text[i] <= 0xFF) {
			buffer[buffer_i++] = (char)text[i];
		}
	}
	buffer[buffer_i] = 0;
}


static struct stringMe {
	char *ptr;
	size_t len;
};

static void init_string(struct stringMe *s) {
	s->len = 0;
	s->ptr = (char*)malloc(s->len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct stringMe *s)
{
	size_t new_len = s->len + size*nmemb;
	s->ptr = (char*)realloc(s->ptr, new_len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr + s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size*nmemb;
}

const int ERROR_CODE_CURL_SOCKET_FAILED = -40;
const int ERROR_CODE_CURL_HANDLE = -41;
const int ERROR_CODE_CURL_EASY_PERF = -42;

int MasterHttpResponse(char* url, char* http_request, char* &rtn_response) {
	int result = ERROR_CODE_CURL_SOCKET_FAILED;//Socket failed to connect to server

	CURL *curl;
	CURLcode res;

	/* In windows, this will init the winsock stuff */
	CURLcode global_init = curl_global_init(CURL_GLOBAL_ALL);
	if (global_init != CURLE_OK) {
		char NotificationPlayerText[100];
		snprintf(NotificationPlayerText, 100, "curl_global_init(CURL_GLOBAL_ALL) failed: %s\n", curl_easy_strerror(global_init));
		addDebugText(NotificationPlayerText);
	}

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) {
		/* First set the URL that is about to receive our POST. This URL can
		just as well be a https:// URL if that is what should receive the
		data. */
		curl_easy_setopt(curl, CURLOPT_URL, url);

		//https://curl.haxx.se/libcurl/c/CURLOPT_PINNEDPUBLICKEY.html
		//not working correctly afaik
		//curl_easy_setopt(curl, CURLOPT_PINNEDPUBLICKEY, "");

		struct stringMe s;
		init_string(&s);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

		/* Now specify the POST data */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, http_request);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			result = ERROR_CODE_CURL_EASY_PERF;//curl_easy_perform() issue
			char NotificationPlayerText[50];
			snprintf(NotificationPlayerText, 50, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			addDebugText(NotificationPlayerText);
			free(s.ptr);
		}
		else {
			rtn_response = s.ptr;
			result = 0;//successful connection.
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	else {
		result = ERROR_CODE_CURL_HANDLE;//curl handle fail
	}
	curl_global_cleanup();

	return result;
}
