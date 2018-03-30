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

bool GetNTStringLine(char* text, int lineNum, char* &line) {
	int inc_line_num = 0;
	char* line_begin = text;

	while (inc_line_num++ < lineNum) {
		line_begin = strchr(line_begin, '\n');
		if (line_begin++ == 0) {
			return false;
		}
	}

	int line_len = 0;
	char* line_next = strchr(line_begin, '\n');
	if (line_next == 0) {
		line_len = strlen(line_begin);
	}
	else {
		if (*(line_next - 1) == '\r') {
			line_next--;
		}
		line_len = line_next - line_begin;
	}

	if (line_len <= 0) {
		line = 0;
		return true;
	}

	line = (char*)malloc(sizeof(char) * (line_len + 1));

	memcpy(line, line_begin, line_len);
	line[line_len] = 0;

	return true;
}

//if missing versioning parameters, major will always be on leftmost side.
char CmpVersions(char* version_base, char* version_alt) {

	if (strcmp(version_base, version_alt) == 0)
		return 0b00000;//same

	int versions[2][4];
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 4; i++) {
			versions[j][i] = 0;
		}
	}

	if (sscanf_s(version_base, "%d.%d.%d.%d", &versions[0][0], &versions[0][1], &versions[0][2], &versions[0][3]) <= 0) {
		return 0b10000;//invalid
	}

	if (sscanf_s(version_alt, "%d.%d.%d.%d", &versions[1][0], &versions[1][1], &versions[1][2], &versions[1][3]) <= 0) {
		return 0b10000;//invalid
	}

	for (int i = 0; i < 4; i++) {
		if (versions[0][i] == versions[1][i]) {
			continue;
		}
		else if (versions[0][i] > versions[1][i]) {//alt is old
			return 0b10000 | (0b1000 >> i);
		}
		else {//alt is new
			return 0b00000 | (0b1000 >> i);
		}
	}

	return 0b00000;//same
	//return 0b01000;//new major
	//return 0b00100;//new minor
	//return 0b00010;//new revision
	//return 0b00001;//new build
	//return 0b11000;//old major
	//return 0b10100;//old minor
	//return 0b10010;//old revision
	//return 0b10001;//old build
}

void ReadIniFile(void* fileConfig, bool configIsFILE, const char* header, char* headerVersion, int(interpretSettingFunc)(char* fileLine, char* version, int lineNumber)) {
	bool foundFirstHeader = false;
	char version[30] = "0";
	bool keepReading = true;
	int lineNumber = 0;
	char* fileLine;
	while (keepReading && ((configIsFILE && GetFileLine((FILE*)fileConfig, fileLine)) || (!configIsFILE && GetNTStringLine((char*)fileConfig, lineNumber, fileLine)))) {
		lineNumber++;
		if (fileLine) {
			if (fileLine[0] == header[0] && sscanf_s(fileLine, header, &version, 30)) {
				foundFirstHeader = true;
				char debugTextBuffer[50];
				snprintf(debugTextBuffer, 50, "Found header on line %d asserting version: %s", lineNumber, version);
				addDebugText(debugTextBuffer);
				if (CmpVersions(headerVersion, version) == 0) {//does not send this line to interpreter.
					free(fileLine);
					continue;
				}
				else {
					addDebugText("Incorrect Version! Continue searching!");
				}
			}
			int rtnCode = interpretSettingFunc(fileLine, foundFirstHeader ? version : 0, lineNumber);
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

int ComputeMd5Hash(const BYTE* buffer, int buflen, char* rtnMd5) {
	for (int i = 0; i < 33; i++) {
		rtnMd5[i] = 0;
	}

	const int MD5LEN = 16;
	DWORD dwStatus = 0;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	BYTE rgbHash[MD5LEN];
	DWORD cbHash = 0;
	CHAR rgbDigits[] = "0123456789abcdef";
	//LPCWSTR filename = L"xinput9_1_0.dll";
	// Logic to check usage goes here.


	// Get handle to the crypto provider
	if (!CryptAcquireContext(&hProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT))
	{
		dwStatus = GetLastError();
		strcpy(rtnMd5, "CryptAcquireContext");
		return dwStatus;
	}

	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
	{
		dwStatus = GetLastError();
		strcpy(rtnMd5, "CryptAcquireContext");
		CryptReleaseContext(hProv, 0);
		return dwStatus;
	}

	if (!CryptHashData(hHash, buffer, buflen, 0))
	{
		dwStatus = GetLastError();
		strcpy(rtnMd5, "CryptHashData");
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
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

bool ComputeFileCrc32Hash(wchar_t* filepath, DWORD &rtncrc32) {

	register DWORD oldcrc32;

	oldcrc32 = 0xFFFFFFFF;

	const int BUFSIZE = 1024;
	BOOL bResult = FALSE;
	HANDLE hFile = NULL;
	BYTE rgbFile[BUFSIZE];
	DWORD cbRead = 0;

	hFile = CreateFile(filepath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return false;
	}

	while (bResult = ReadFile(hFile, rgbFile, BUFSIZE, &cbRead, NULL))
	{
		if (cbRead == 0) {
			break;
		}

		for (int i = 0; i < cbRead; i++) {
			oldcrc32 = UPDC32(rgbFile[i], oldcrc32);
		}
	}

	if (!bResult) {
		return false;
	}

	CloseHandle(hFile);

	rtncrc32 = ~oldcrc32;

	return true;
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

char* encode_rfc3986(char* label_literal, int label_literal_length) {
	if (label_literal_length < 0)
		label_literal_length = strlen(label_literal);
	int escaped_buflen = (label_literal_length * 3) + 1;
	char* label_escaped = (char*)malloc(escaped_buflen * sizeof(char));
	int escaped_buff_i = 0;

	for (int i = 0; i < label_literal_length; i++) {
		unsigned char uletter = label_literal[i];
		if (!rfc3986_allow(uletter)) {
			sprintf_s(label_escaped + escaped_buff_i, 4, "%%%02X", uletter);
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
		snprintf(NotificationPlayerText, 100, "curl_global_init(CURL_GLOBAL_ALL) failed: %s", curl_easy_strerror(global_init));
		addDebugText(NotificationPlayerText);
	}

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) {
		/* First set the URL that is about to receive our POST. This URL can
		just as well be a https:// URL if that is what should receive the
		data. */
		curl_easy_setopt(curl, CURLOPT_URL, url);

		struct stringMe s;
		init_string(&s);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		//FIXME: <Insert Pinned Public Key Here>

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

		/* Now specify the POST data */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, http_request);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			result = ERROR_CODE_CURL_EASY_PERF;//curl_easy_perform() issue
			char NotificationPlayerText[500];
			snprintf(NotificationPlayerText, 500, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
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

bool StrnCaseInsensEqu(char* str1, char* str2, unsigned int chk_len) {
	int chk_len2 = strlen(str1);
	if (chk_len2 < chk_len) {
		chk_len = chk_len2;
	}
	chk_len2 = strlen(str2);
	if (chk_len2 != chk_len) {
		return false;
	}
	const int case_diff = 'A' - 'a';
	for (int i = 0; i < chk_len; i++) {
		if (str1[i] != str2[i]) {
			int sa = str1[i];
			if (sa >= 'a' && sa <= 'z') {
				sa += case_diff;
			}
			else if (sa >= 'A' && sa <= 'Z') {
				sa -= case_diff;
			}
			if (sa == str2[i]) {
				continue;
			}
			return false;
		}
	}

	return true;
}

void EnsureDirectoryExists(wchar_t* path) {
	int buflen = wcslen(path) + 1;
	wchar_t* path2 = (wchar_t*)malloc(sizeof(wchar_t) * buflen);
	memcpy(path2, path, sizeof(wchar_t) * buflen);

	for (int i = 1; i < buflen; i++) {
		if (path2[i] == L'/' || path2[i] == L'\\') {
			wchar_t temp_cut = 0;
			if (path2[i + 1] != 0) {
				temp_cut = path2[i + 1];
				path2[i + 1] = 0;
			}
			CreateDirectoryW(path2, NULL);
			if (temp_cut) {
				path2[i + 1] = temp_cut;
			}
		}
	}

	free(path2);
}

int TrimRemoveConsecutiveSpaces(char* text) {
	int text_len = strlen(text);
	int text_pos = 0;
	for (int j = 0; j < text_len; j++) {
		if (text_pos == 0) {
			if (text[j] != ' ')
				text[text_pos++] = text[j];
			continue;
		}
		if (!(text[j] == ' ' && text[text_pos - 1] == ' '))
			text[text_pos++] = text[j];
	}
	text[text_pos] = 0;
	if (text[text_pos - 1] == ' ')
		text[--text_pos] = 0;
	return text_pos;//new length
}

