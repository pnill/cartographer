#include "H2Accounts.h"
#include "GSUtils.h"
#include "H2OnscreenDebugLog.h"
#include "H2Config.h"
#include "H2Startup.h"

static void HandleFileError(int fpErrNo) {//TODO
	if (fpErrNo == EACCES || fpErrNo == EIO || fpErrNo == EPERM) {
		MessageBoxA(NULL, "Cannot write a file. Please restart Halo 2 in Administrator mode!", "Permission Error!", MB_OK);
	}
	else if (fpErrNo == ESRCH) {
		MessageBoxA(NULL, "Probably a missing folder issue if file writing related. Please restart Halo 2 in Administrator mode!", "Permission Error!", MB_OK);
	}
	else {
		char NotificationPlayerText[20];
		sprintf(NotificationPlayerText, "Error 0x%x", fpErrNo);
		addDebugText(NotificationPlayerText);
		MessageBoxA(NULL, NotificationPlayerText, "Unknown File Failure!", MB_OK);
	}
}

#pragma region Config IO
const wchar_t H2AccountsFilename[] = L"%wshalo2accounts.ini";

const char H2ConfigVersionStr[] = "[H2AccountsVersion:%d]";
const int H2ConfigVersion = 1;
const char H2ConfigAccountStr[] = "[Account]";

const int bufferIncSize = 10;
static int oldConfigBufferI = 0;
static int oldConfigBufferLen = 0;
static char** oldConfigBuffer = 0;
static int badConfigBufferI = 0;
static int badConfigBufferLen = 0;
static char** badConfigBuffer = 0;
int H2AccountCount = 0;
int H2AccountBufferI = -1;
static int H2AccountBufferLen = 0;
char** H2AccountBufferUsername = 0;
char** H2AccountBufferLoginToken = 0;
int H2AccountLastUsed = 0;
static bool H2AccountBufferUsername_est = false;
static bool H2AccountBufferLoginToken_est = false;
static bool H2AccountLastUsed_est = false;

char* H2CurrentAccountLoginToken = 0;

static void oldConfigBufferFree() {
	if (oldConfigBuffer) {
		for (int i = 0; i < oldConfigBufferI; i++) {
			free(oldConfigBuffer[i]);
		}
		oldConfigBufferI = 0;
		oldConfigBufferLen = 0;
		free(oldConfigBuffer);
		oldConfigBuffer = 0;
	}
}
static void badConfigBufferFree() {
	if (badConfigBuffer) {
		for (int i = 0; i < badConfigBufferI; i++) {
			free(badConfigBuffer[i]);
		}
		badConfigBufferI = 0;
		badConfigBufferLen = 0;
		free(badConfigBuffer);
		badConfigBuffer = 0;
	}
}
static void H2AccountBufferFree() {
	if (H2AccountBufferUsername) {
		for (int i = 0; i < H2AccountBufferI; i++) {
			if (H2AccountBufferUsername[i])
				free(H2AccountBufferUsername[i]);
		}
		free(H2AccountBufferUsername);
		H2AccountBufferUsername = 0;
	}
	if (H2AccountBufferLoginToken) {
		for (int i = 0; i < H2AccountBufferI; i++) {
			if (H2AccountBufferLoginToken[i])
				free(H2AccountBufferLoginToken[i]);
		}
		free(H2AccountBufferLoginToken);
		H2AccountBufferLoginToken = 0;
	}
	H2AccountBufferI = -1;
	H2AccountBufferLen = 0;
}

HANDLE H2Accounts_mutex = 0;

void ReleaseAccountConfigLock() {
	if (H2Accounts_mutex) {
		ReleaseMutex(H2Accounts_mutex);
		CloseHandle(H2Accounts_mutex);
		H2Accounts_mutex = 0;
	}
}

bool TestAccountConfigLock() {
	return H2Accounts_mutex != 0;
}

bool TestGetAccountConfigLock(wchar_t* mutexName) {

	if (H2Accounts_mutex) {
		return true;
	}

	DWORD crc32num = crc32buf((char*)mutexName, wcslen(mutexName) * 2);
	wchar_t crc32mutexName[40] = { L"" };
	swprintf(crc32mutexName, 40, L"Halo2AccountsFile-%x", crc32num);

	HANDLE mutex = CreateMutexW(0, true, crc32mutexName);
	DWORD lastErr = GetLastError();
	if (lastErr == ERROR_ALREADY_EXISTS) {
		CloseHandle(mutex);
		mutex = 0;
	}

	if (mutex)
		H2Accounts_mutex = mutex;

	return TestAccountConfigLock();
}

void SaveH2Accounts() {
	addDebugText("Saving H2Accounts File...");

	if (TestAccountConfigLock()) {
		addDebugText("Mutex is Ours!");

		wchar_t fileConfigPath[1024];
		if (H2Portable) {
			swprintf(fileConfigPath, 1024, H2AccountsFilename, H2ProcessFilePath);
		}
		else {
			swprintf(fileConfigPath, 1024, H2AccountsFilename, H2AppDataLocal);
		}

		wchar_t fileConfigPathLog[1124];
		swprintf(fileConfigPathLog, 1024, L"Saving Accounts: \"%ws\"", fileConfigPath);
		addDebugText(fileConfigPathLog);
		FILE* fileConfig = _wfopen(fileConfigPath, L"wb");

		if (fileConfig == NULL) {
			HandleFileError(GetLastError());
			addDebugText("ERROR: Unable to write H2Accounts File!");
		}
		else {
#pragma region Put Data To File
			fputs("#--- Halo 2 Project Cartographer Accounts File ---", fileConfig);
			fputs("\n\n", fileConfig);
			fputs("# DO NOT SHARE THE CONTENTS OF THIS FILE.", fileConfig);
			fputs("\n\n", fileConfig);

			int fputbufferlen = strlen(H2ConfigVersionStr) + 1;
			char* fputbuffer = (char*)malloc(sizeof(char) * fputbufferlen);
			snprintf(fputbuffer, fputbufferlen, H2ConfigVersionStr, H2ConfigVersion);
			fputs(fputbuffer, fileConfig);
			free(fputbuffer);

			fputs("\n", fileConfig);

			char last_used_buff[30];
			snprintf(last_used_buff, 30, "\nlast_used = %d\n", H2AccountLastUsed);
			fputs(last_used_buff, fileConfig);

			if (H2AccountBufferLoginToken && H2AccountBufferUsername && H2AccountCount > 0) {
				for (int i = 0; i < H2AccountCount; i++) {
					fputs("\n", fileConfig); fputs(H2ConfigAccountStr, fileConfig);
					if (H2AccountBufferUsername[i]) {
						fputs("\nusername = ", fileConfig); fputs(H2AccountBufferUsername[i], fileConfig);
					}
					fputs("\nlogin_token = ", fileConfig); fputs(H2AccountBufferLoginToken[i] ? H2AccountBufferLoginToken[i] : "", fileConfig);
					fputs("\n", fileConfig);
				}
			}
			else {
				fputs("\n", fileConfig); fputs(H2ConfigAccountStr, fileConfig);
				fputs("\nlogin_token = ", fileConfig);
			}

			fputs("\n", fileConfig);

			if (badConfigBuffer && badConfigBufferI > 0) {
				for (int i = 0; i < badConfigBufferI; i++) {
					fputs("\n", fileConfig);
					if (badConfigBuffer[i][0] != '#' && badConfigBuffer[i][0] != ';')
						fputs("#", fileConfig);
					fputs(badConfigBuffer[i], fileConfig);
				}
			}
			badConfigBufferFree();

			fputs("\n\n", fileConfig);

			if (oldConfigBuffer && oldConfigBufferI > 0) {
				for (int i = 0; i < oldConfigBufferI; i++) {
					fputs("\n", fileConfig);
					fputs(oldConfigBuffer[i], fileConfig);
				}
			}
			oldConfigBufferFree();

			fputs("\n", fileConfig);
#pragma endregion
			fclose(fileConfig);
		}
		ReleaseAccountConfigLock();
	}
	else {
		addDebugText("Mutex Not Held!");
	}

	addDebugText("End Saving H2Accounts File.");
}

void H2AccountBufferCheck() {
	if (H2AccountBufferI >= H2AccountBufferLen) {
		H2AccountBufferLen += bufferIncSize;
		if (!H2AccountBufferLoginToken) {
			H2AccountBufferUsername = (char**)malloc(sizeof(char*) * H2AccountBufferLen);
			H2AccountBufferLoginToken = (char**)malloc(sizeof(char*) * H2AccountBufferLen);
		}
		else {
			H2AccountBufferUsername = (char**)realloc(H2AccountBufferUsername, sizeof(char*) * H2AccountBufferLen);
			H2AccountBufferLoginToken = (char**)realloc(H2AccountBufferLoginToken, sizeof(char*) * H2AccountBufferLen);
		}
		for (int i = H2AccountBufferLen - bufferIncSize; i < H2AccountBufferLen; i++) {
			H2AccountBufferUsername[i] = 0;
			H2AccountBufferLoginToken[i] = 0;
		}
	}
}

void H2AccountBufferAdd(char* token, char* username) {
	int bufflen;
	H2AccountBufferCheck();

	bufflen = strlen(token) + 1;
	H2AccountBufferLoginToken[H2AccountBufferI] = (char*)malloc(sizeof(char) * bufflen);
	snprintf(H2AccountBufferLoginToken[H2AccountBufferI], bufflen, token);

	bufflen = strlen(username) + 1;
	H2AccountBufferUsername[H2AccountBufferI] = (char*)malloc(sizeof(char) * bufflen);
	snprintf(H2AccountBufferUsername[H2AccountBufferI], bufflen, username);

	H2AccountBufferI++;
	H2AccountCount++;
}

static int interpretConfigSetting(char* fileLine, int version, int lineNumber) {
	if (version != H2ConfigVersion) {
		if (oldConfigBufferI >= oldConfigBufferLen) {
			if (!oldConfigBuffer) {
				oldConfigBuffer = (char**)malloc(sizeof(char*) * (oldConfigBufferLen += bufferIncSize));
			}
			else {
				oldConfigBuffer = (char**)realloc(oldConfigBuffer, sizeof(char*) * (oldConfigBufferLen += bufferIncSize));
			}
		}
		oldConfigBuffer[oldConfigBufferI] = fileLine;
		oldConfigBufferI++;
		return 0b10;
	}
	else {
		bool unrecognised = false;
		bool duplicated = false;
		bool incorrect = false;
		bool dontSave = false;
		int fileLineLen = strlen(fileLine);
		int tempint1 = -1;
		unsigned short tempushort1 = -1;
		int tempint2 = -1;
		float tempfloat1 = NULL;
		char tempstr1[33] = { "" };
		if (fileLine[0] == '#' || fileLine[0] == ';' || fileLineLen <= 2) {
			unrecognised = true;
		}
		else if (strstr(fileLine, H2ConfigAccountStr)) {
			if (!H2AccountBufferLoginToken_est && H2AccountBufferI >= 0) {
				incorrect = true;
				dontSave = true;
			}
			else {
				H2AccountBufferI++;
				H2AccountBufferUsername_est = false;
				H2AccountBufferLoginToken_est = false;
				H2AccountBufferCheck();
			}
		}
		else if (strstr(fileLine, "username =")) {
			if (H2AccountBufferUsername_est) {
				duplicated = true;
			}
			else if (H2AccountBufferI < 0) {
				incorrect = true;
			}
			else {
				char* tempName = fileLine + strlen("username =");
				while (isspace(*tempName)) {
					tempName++;
				}
				snprintf(tempstr1, 17, tempName);
				for (int j = strlen(tempstr1) - 1; j > 0; j--) {
					if (isspace(tempstr1[j])) {
						tempstr1[j] = 0;
					}
					else {
						break;
					}
				}
				int bufflen = strlen(tempstr1) + 1;
				if (bufflen <= 1) {
					incorrect = true;
				}
				else {
					H2AccountBufferUsername[H2AccountBufferI] = (char*)malloc(sizeof(char) * bufflen);
					snprintf(H2AccountBufferUsername[H2AccountBufferI], bufflen, tempstr1);
					H2AccountBufferUsername_est = true;
				}
			}
		}
		else if (strstr(fileLine, "login_token =")) {
			if (H2AccountBufferLoginToken_est) {
				duplicated = true;
			}
			else if (H2AccountBufferI < 0) {
				incorrect = true;
			}
			else {
				char* tempName = fileLine + strlen("login_token =");
				while (isspace(*tempName)) {
					tempName++;
				}
				snprintf(tempstr1, 33, tempName);
				for (int j = strlen(tempstr1) - 1; j > 0; j--) {
					if (isspace(tempstr1[j])) {
						tempstr1[j] = 0;
					}
					else {
						break;
					}
				}
				int bufflen = strlen(tempstr1) + 1;
				if (bufflen <= 1) {
					//incorrect = true;
				}
				else {
					H2AccountBufferLoginToken[H2AccountBufferI] = (char*)malloc(sizeof(char) * bufflen);
					snprintf(H2AccountBufferLoginToken[H2AccountBufferI], bufflen, tempstr1);
					H2AccountBufferLoginToken_est = true;
				}
			}
		}
		else if (sscanf(fileLine, "last_used =%d", &tempint1) == 1) {
			if (H2AccountLastUsed_est) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2AccountLastUsed = (bool)tempint1;
				H2AccountLastUsed_est = true;
			}
		}
		else {
			unrecognised = true;
		}

		if (unrecognised || duplicated || incorrect) {
			char textDebugBuffer[60];
			if (duplicated) {
				snprintf(textDebugBuffer, 60, "ERROR: Duplicated Config Setting on Line: %d", lineNumber);
				addDebugText(textDebugBuffer);
			}
			else if (incorrect) {
				snprintf(textDebugBuffer, 60, "ERROR: Incorrect Config Setting on Line: %d", lineNumber);
				addDebugText(textDebugBuffer);
			}
			if (!dontSave) {
				if (badConfigBufferI >= badConfigBufferLen) {
					if (!badConfigBuffer) {
						badConfigBuffer = (char**)malloc(sizeof(char*) * (badConfigBufferLen += bufferIncSize));
					}
					else {
						badConfigBuffer = (char**)realloc(badConfigBuffer, sizeof(char*) * (badConfigBufferLen += bufferIncSize));
					}
				}
				badConfigBuffer[badConfigBufferI] = fileLine;
				badConfigBufferI++;
				return 0b10;
			}
		}
	}
	return 0;
}

bool ReadH2Accounts() {
	addDebugText("Reading H2Accounts File...");

	wchar_t fileConfigPath[1024];

	if (H2Portable) {
		swprintf(fileConfigPath, 1024, H2AccountsFilename, H2ProcessFilePath);
	}
	else {
		swprintf(fileConfigPath, 1024, H2AccountsFilename, H2AppDataLocal);
	}

	wchar_t fileConfigPathLog[1124];
	swprintf(fileConfigPathLog, 1024, L"Reading Accounts: \"%ws\"", fileConfigPath);
	addDebugText(fileConfigPathLog);
	if (TestGetAccountConfigLock(fileConfigPath)) {
		addDebugText("Mutex Obtained!");

		FILE* fileConfig = _wfopen(fileConfigPath, L"rb");

		if (!fileConfig) {
			addDebugText("ERROR: No H2Accounts Files Could Be Found!");
		}
		else {

			oldConfigBufferFree();
			badConfigBufferFree();
			H2AccountBufferFree();

			H2AccountLastUsed_est = false;

			ReadIniFile(fileConfig, true, H2ConfigVersionStr, H2ConfigVersion, interpretConfigSetting);

			if (H2AccountBufferLoginToken[H2AccountBufferI]) {
				H2AccountBufferI++;
			}

			H2AccountCount = H2AccountBufferI;

			fclose(fileConfig);
		}
	}
	else {
		addDebugText("Mutex In use!");
	}

	if (H2AccountBufferI < 0) {
		H2AccountBufferI = 0;
	}

	addDebugText("End Reading H2Accounts File.");

	return TestAccountConfigLock();
}
#pragma endregion

#pragma region Config Init/Deinit
void InitH2Accounts() {
	//ReadH2Accounts();
}
void DeinitH2Accounts() {
	//SaveH2Accounts();
}
#pragma endregion
