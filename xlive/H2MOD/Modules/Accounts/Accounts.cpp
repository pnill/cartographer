
#include "H2MOD\Modules\Accounts\Accounts.h"

#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Startup\Startup.h"

#include "Util\SimpleIni.h"

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

static const std::string H2ConfigAccountStr = "Account:";

static const std::string H2AccConfigVersion = "1";
static const std::string H2AccConfigVersionStr = "H2AccountsVersion:" + H2AccConfigVersion;

int H2AccountCount = 0;
static int H2AccountArrayLen = 0;
char** H2AccountArrayUsername = nullptr;
char** H2AccountArrayLoginToken = nullptr;
int H2AccountLastUsed = 0;
int arrayIncSize = 10;

char* H2CurrentAccountLoginToken = nullptr;

static void H2AccountBufferFree() {
	if (H2AccountArrayUsername) {
		for (int i = 0; i < H2AccountCount; i++) {
			if (H2AccountArrayUsername[i])
				free(H2AccountArrayUsername[i]);
		}
		free(H2AccountArrayUsername);
		H2AccountArrayUsername = nullptr;
	}
	if (H2AccountArrayLoginToken) {
		for (int i = 0; i < H2AccountCount; i++) {
			if (H2AccountArrayLoginToken[i])
				free(H2AccountArrayLoginToken[i]);
		}
		free(H2AccountArrayLoginToken);
		H2AccountArrayLoginToken = nullptr;
	}
	H2AccountArrayLen = 0;
	H2AccountCount = 0;
}

HANDLE H2Accounts_mutex = INVALID_HANDLE_VALUE;

void ReleaseAccountConfigLock() {
	if (H2Accounts_mutex) {
		ReleaseMutex(H2Accounts_mutex);
		CloseHandle(H2Accounts_mutex);
		H2Accounts_mutex = INVALID_HANDLE_VALUE;
	}
}

bool TestAccountConfigLock() {
	return H2Accounts_mutex != INVALID_HANDLE_VALUE;
}

bool TestGetAccountConfigLock(wchar_t* mutexName) {

	if (H2Accounts_mutex != INVALID_HANDLE_VALUE) {
		return true;
	}

	DWORD crc32num = crc32buf((char*)mutexName, wcslen(mutexName) * 2);
	wchar_t crc32mutexName[40] = { L"" };
	swprintf(crc32mutexName, ARRAYSIZE(crc32mutexName), L"Halo2AccountsFile-%x", crc32num);

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
			swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), H2AccountsFilename, H2ProcessFilePath);
		}
		else {
			swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), H2AccountsFilename, H2AppDataLocal);
		}

		wchar_t fileConfigPathLog[1124];
		swprintf(fileConfigPathLog, 1024, L"Saving Accounts: \"%ws\"", fileConfigPath);
		addDebugText(fileConfigPathLog);
		FILE* fileConfig = _wfopen(fileConfigPath, L"wb");

		if (fileConfig == nullptr) {
			HandleFileError(GetLastError());
			addDebugText("ERROR: Unable to write H2Accounts File!");
		}
		else {
#pragma region Put Data To File
			std::fstream fileStream(fileConfig);
			CSimpleIniA ini;

			fileStream <<
				"#--- Halo 2 Project Cartographer Accounts File ---"
				"\n\n"
				"# DO NOT SHARE THE CONTENTS OF THIS FILE."
				"\n\n"
				;

			ini.SetLongValue(H2AccConfigVersionStr.c_str(), "last_used", H2AccountLastUsed);
			ini.SetLongValue(H2AccConfigVersionStr.c_str(), "account_count", H2AccountCount);

			if (H2AccountArrayLoginToken && H2AccountArrayUsername && H2AccountCount > 0) {
				for (int i = 0; i < H2AccountCount; i++) {
					ini.SetValue((H2ConfigAccountStr + std::to_string(i + 1)).c_str(), "username", H2AccountArrayUsername[i] ? H2AccountArrayUsername[i] : "");
					ini.SetValue((H2ConfigAccountStr + std::to_string(i + 1)).c_str(), "login_token", H2AccountArrayLoginToken[i] ? H2AccountArrayLoginToken[i] : "");
				}
			}

			ini.SaveFile(fileConfig);

			/*if (badConfigBuffer && badConfigBufferI > 0) {
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

			fputs("\n", fileConfig);*/

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
	if (H2AccountCount >= H2AccountArrayLen) {
		H2AccountArrayLen += arrayIncSize;
		if (!H2AccountArrayLoginToken) {
			H2AccountArrayUsername = (char**)calloc(H2AccountArrayLen, sizeof(char*));
			H2AccountArrayLoginToken = (char**)calloc(H2AccountArrayLen, sizeof(char*));
		}
		else {
			H2AccountArrayUsername = (char**)realloc(H2AccountArrayUsername, sizeof(char*) * H2AccountArrayLen);
			H2AccountArrayLoginToken = (char**)realloc(H2AccountArrayLoginToken, sizeof(char*) * H2AccountArrayLen);
		}
		for (int i = H2AccountArrayLen - arrayIncSize; i < H2AccountArrayLen; i++) { // dont look, this is G
			H2AccountArrayUsername[i] = 0;
			H2AccountArrayLoginToken[i] = 0;
		}
	}
}

void H2AccountAccountAdd(const char* username, const char* token) {
	int bufflen;

	// verify if the same credentials already exist
	for (int i = 0; i < H2AccountCount; i++) {
		if (StrnCaseInsensEqu(H2AccountArrayUsername[i], (char*)username, XUSER_MAX_NAME_LENGTH)) {
			
			if (H2AccountArrayLoginToken[i]) {
				free(H2AccountArrayLoginToken[i]);
			}
			if (H2AccountArrayUsername[i]) {
				free(H2AccountArrayUsername[i]);
			}

			bufflen = strlen(token) + 1;
			H2AccountArrayLoginToken[i] = (char*)calloc(bufflen, sizeof(*token));
			snprintf(H2AccountArrayLoginToken[i], bufflen, token);

			H2AccountArrayUsername[i] = (char*)calloc(XUSER_NAME_SIZE, sizeof(*username));
			strncpy_s(H2AccountArrayUsername[i], XUSER_NAME_SIZE, username, strnlen_s(username, XUSER_MAX_NAME_LENGTH));

			return;
		}
	}

	H2AccountCount++;
	H2AccountBufferCheck();
	int H2AccountArrayIndex = H2AccountCount - 1;

	// allocate new token/username pair if it doesn't already exist
	bufflen = strlen(token) + 1;
	H2AccountArrayLoginToken[H2AccountArrayIndex] = (char*)calloc(bufflen, sizeof(*token));
	snprintf(H2AccountArrayLoginToken[H2AccountArrayIndex], bufflen, token);

	H2AccountArrayUsername[H2AccountArrayIndex] = (char*)calloc(XUSER_NAME_SIZE, sizeof(*username));
	strncpy_s(H2AccountArrayUsername[H2AccountArrayIndex], XUSER_NAME_SIZE, username, strnlen_s(username, XUSER_MAX_NAME_LENGTH));
}

void H2AccountAccountRemove(int accountArrayIndex)
{
	if (H2AccountCount > 0)
	{
		if (H2AccountArrayLoginToken) {
			if (H2AccountArrayLoginToken[accountArrayIndex]) {
				free(H2AccountArrayLoginToken[accountArrayIndex]);
			}
			for (int i = accountArrayIndex + 1; i < H2AccountCount; i++) {
				H2AccountArrayLoginToken[i - 1] = H2AccountArrayLoginToken[i];
			}

			if (H2AccountArrayUsername) {
				if (H2AccountArrayUsername[accountArrayIndex]) {
					free(H2AccountArrayUsername[accountArrayIndex]);
				}
				for (int i = accountArrayIndex + 1; i < H2AccountCount; i++) {
					H2AccountArrayUsername[i - 1] = H2AccountArrayUsername[i];
				}
			}
			H2AccountCount--;
		}
	}
}

bool ReadH2Accounts() {
	addDebugText("Reading H2Accounts File...");

	wchar_t fileConfigPath[1024];

	if (H2Portable) {
		swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), H2AccountsFilename, H2ProcessFilePath);
	}
	else {
		swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), H2AccountsFilename, H2AppDataLocal);
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

			//clear the buffers before reading any accounts from the file
			H2AccountBufferFree();

			CSimpleIniA ini;
			ini.SetUnicode();

			// load the file in SimpleIni
			SI_Error rc = ini.LoadFile(fileConfig);
			if (rc < 0)
			{
				addDebugText(std::string("ini.LoadFile() failed with error: " + std::to_string(rc) + " while reading H2 accounts file").c_str());
			}
			else
			{
				// read the accounts in the file
				H2AccountLastUsed = ini.GetLongValue(H2AccConfigVersionStr.c_str(), "last_used", H2AccountLastUsed);
				int AccountCount = ini.GetLongValue(H2AccConfigVersionStr.c_str(), "account_count", 0);
				if (AccountCount > 0)
				{
					for (int i = 0; i < AccountCount; i++)
					{
						const char* username = ini.GetValue((H2ConfigAccountStr + std::to_string(i + 1)).c_str(), "username", "");
						const char* login_token = ini.GetValue((H2ConfigAccountStr + std::to_string(i + 1)).c_str(), "login_token", "");

						H2AccountAccountAdd(username, login_token);
					}
				}
			}

			fclose(fileConfig);
		}
	}
	else {
		addDebugText("Mutex In use!");
	}

	addDebugText("End Reading H2Accounts File.");

	return TestAccountConfigLock();
}
#pragma endregion

#pragma region Config Init/Deinit
void InitH2Accounts() {

}
void DeinitH2Accounts() {

}
#pragma endregion
