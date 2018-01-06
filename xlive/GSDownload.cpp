#include "GSDownload.h"
#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include "GSUtils.h"
#include "H2OnscreenDebugLog.h"
#include <vector>
#include "H2Startup.h"
#include "GSCustomLanguage.h"
#include "GSCustomMenu.h"

bool fork_cmd_elevate(const char* cmd, char* flags = 0) {
	SHELLEXECUTEINFO shExInfo = { 0 };
	shExInfo.cbSize = sizeof(shExInfo);
	shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExInfo.hwnd = 0;
	shExInfo.lpVerb = "runas";                // Operation to perform
	shExInfo.lpFile = cmd;       // Application to start    
	shExInfo.lpParameters = flags ? flags : "";                  // Additional parameters
	shExInfo.lpDirectory = 0;
	shExInfo.nShow = SW_SHOW;
	shExInfo.hInstApp = 0;

	if (ShellExecuteEx(&shExInfo))
	{
		//WaitForSingleObject(shExInfo.hProcess, INFINITE);
		CloseHandle(shExInfo.hProcess);
		return true;
	}
	return false;
}

bool fork_cmd(LPSTR cmd) {

	PROCESS_INFORMATION piProcInfo;
	STARTUPINFOA siStartInfo;
	bool bSuccess;

	// Set up members of the PROCESS_INFORMATION structure. 
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDERR and STDOUT handles for redirection.
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	//siStartInfo.dwFlags |= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//siStartInfo.wShowWindow = SW_HIDE; // execute hide 

	// Create the child process. 
	bSuccess = CreateProcessA(//NULL,
		NULL,     // command line 
		cmd,
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION

	return bSuccess;
}

long long Size_Of_Download = 0;
long long Size_Of_Downloaded = 0;

static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	Size_Of_Downloaded += nmemb;
	return written;
}


static size_t header_callback(char* buffer, size_t size, size_t nitems, void* userdata) {
	/* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
	/* 'userdata' is set with CURLOPT_HEADERDATA */

	sscanf(buffer, "Content-Length:%lld", &Size_Of_Download);
	return nitems * size;
}

int DownloadFile(const char* url, wchar_t* local_full_path) {
	Size_Of_Download = Size_Of_Downloaded = 0;
	CURL *curl;
	FILE *fp;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
		EnsureDirectoryExists(local_full_path);
		fp = _wfopen(local_full_path, L"wb");
		if (!fp) {
			addDebugText("Failed to obtain FILE* for DL from: to: ");
			addDebugText(url);
			addDebugText(local_full_path);
			return 2;
		}
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		//FIXME: <Insert Pinned Public Key Here>
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
		Size_Of_Download = Size_Of_Downloaded = 0;
		return 0;
	}
	return 1;
}

typedef struct {
	char need_to_update;
	bool core_file;
	char location_id;
	char* local_name;
	char* server_uri;
	DWORD crc32;
} UpdateFileEntry;

std::vector<UpdateFileEntry*> UpdateFileEntries;

static void freeUpdateFileEntry(UpdateFileEntry* update_file_entry) {
	free(update_file_entry->local_name);
	free(update_file_entry->server_uri);
	free(update_file_entry);
}

static const char H2UpdateVersionStr[] = "[Version:%[^]]]";
static const char H2UpdateLocationsStr[3][10] = { "[Temp]", "[Game]", "[AppData]" };
static char H2UpdateVersion[30] = "0";
static char current_location_id = 0;

bool GSDownload_files_to_download = false;
bool GSDownload_files_to_install = false;

static void setButtonState(unsigned int btn_ID, char button_state) {
	if (button_state == 0)
		add_cartographer_label(CMLabelMenuId_Update, btn_ID, (char*)0, true);
	else if (button_state == 1)
		add_cartographer_label(CMLabelMenuId_Update, btn_ID, H2CustomLanguageGetLabel(CMLabelMenuId_Update, 0xFFFF0000 + btn_ID), true);
	else if (button_state == 2)
		add_cartographer_label(CMLabelMenuId_Update, btn_ID, H2CustomLanguageGetLabel(CMLabelMenuId_Update, 0xFFFFFF00 + btn_ID), true);
}


static int interpretUpdateEntry(char* fileLine, char* version, int lineNumber) {
	bool read_it = false;

	if (!version) {
		read_it = true;
	}
	else {
		char rtn_result = CmpVersions(H2UpdateVersion, version);
		if (rtn_result == 0) {
			read_it = true;
		}
		else if (rtn_result & 0b10000) {

		}
		else {
			strcpy_s(H2UpdateVersion, version);
			int entry_count = UpdateFileEntries.size();
			for (int i = entry_count-1; i >= 0; i--) {
				if (!UpdateFileEntries[i]->core_file) {
					freeUpdateFileEntry(UpdateFileEntries[i]);
					UpdateFileEntries.erase(UpdateFileEntries.begin() + i);
				}
			}
			read_it = true;
		}
	}

	if (read_it) {//and weep
		bool unrecognised = false;
		bool duplicated = false;
		bool incorrect = false;
		bool dontSave = false;
		int fileLineLen = strlen(fileLine);
		int tempint1 = -1;
		DWORD tempDWORD = 0;
		unsigned short tempushort1 = -1;
		int tempint2 = -1;
		float tempfloat1 = NULL;
		char tempstr1[260] = "";
		char tempstr2[260] = "";
		if (fileLine[0] == '#' || fileLine[0] == ';' || fileLineLen < 5) {
			unrecognised = true;
		}
		else if (strstr(fileLine, H2UpdateLocationsStr[0])) {
			current_location_id = 0;
		}
		else if (strstr(fileLine, H2UpdateLocationsStr[1])) {
			current_location_id = 1;
		}
		else if (strstr(fileLine, H2UpdateLocationsStr[2])) {
			current_location_id = 2;
		}
		else if (sscanf_s(fileLine, "%[^=]=%zx=%s", &tempstr1, 260, &tempDWORD, &tempstr2, 260) == 3) {
			
			UpdateFileEntry* update_file_entry = (UpdateFileEntry*)malloc(sizeof(UpdateFileEntry));

			update_file_entry->need_to_update = 2;

			update_file_entry->core_file = version == 0;

			update_file_entry->location_id = current_location_id;

			update_file_entry->crc32 = tempDWORD;

			int namebufflen = strlen(tempstr1) + 1;
			update_file_entry->local_name = (char*)malloc(namebufflen);
			memcpy(update_file_entry->local_name, tempstr1, namebufflen);

			namebufflen = strlen(tempstr2) + 1;
			update_file_entry->server_uri = (char*)malloc(namebufflen);
			memcpy(update_file_entry->server_uri, tempstr2, namebufflen);

			bool save = true;

			int entry_count = UpdateFileEntries.size();
			for (int i = entry_count - 1; i >= 0; i--) {
				if (UpdateFileEntries[i]->core_file == update_file_entry->core_file && UpdateFileEntries[i]->location_id == update_file_entry->location_id && strcmp(UpdateFileEntries[i]->local_name, update_file_entry->local_name) == 0) {
					freeUpdateFileEntry(update_file_entry);
					save = false;
					break;
				}
			}
			
			if (save)
				UpdateFileEntries.push_back(update_file_entry);
			else
				duplicated = true;

			//incorrect = true;
		}
		else {
			unrecognised = true;
		}

		if (unrecognised || duplicated || incorrect) {
			char textDebugBuffer[60];
			if (duplicated) {
				snprintf(textDebugBuffer, 60, "ERROR: Duplicated update file on Line: %d", lineNumber);
				addDebugText(textDebugBuffer);
			}
			else if (incorrect) {
				snprintf(textDebugBuffer, 60, "ERROR: Incorrect update file on Line: %d", lineNumber);
				addDebugText(textDebugBuffer);
			}
		}
	}
	return 0;
}


static void FetchUpdateDetails() {
	wchar_t* dir_temp = _wgetenv(L"TEMP");

	wchar_t dir_temp_h2[1024];
	swprintf(dir_temp_h2, 1024, L"%ws\\Halo2\\", dir_temp);
	CreateDirectoryW(dir_temp_h2, NULL);

	wchar_t dir_update[1024];
	swprintf(dir_update, 1024, L"%ws\\Halo2\\Update\\", dir_temp);
	CreateDirectoryW(dir_update, NULL);

	wchar_t dir_update_old[1024];
	swprintf(dir_update_old, 1024, L"%ws\\Halo2\\UpdateOld\\", dir_temp);
	CreateDirectoryW(dir_update_old, NULL);

	int entry_count = UpdateFileEntries.size();
	for (int i = entry_count - 1; i >= 0; i--) {
		freeUpdateFileEntry(UpdateFileEntries[i]);
	}
	UpdateFileEntries.clear();
	current_location_id = 0;
	H2UpdateVersion[0] = '0';
	H2UpdateVersion[1] = 0;

	GSDownload_files_to_download = false;
	GSDownload_files_to_install = false;

	addDebugText("Fetching Update Details.");
	char* rtn_result = 0;
	int rtn_code = MasterHttpResponse("https://cartographer.online/update1.ini", "", rtn_result);
	if (rtn_code == 0) {
		addDebugText("Got Update Details.");

		ReadIniFile(rtn_result, false, H2UpdateVersionStr, "0", interpretUpdateEntry);

		free(rtn_result);

		addDebugText("Interpreted the details.");

		int entry_count = UpdateFileEntries.size();
		for (int i = entry_count - 1; i >= 0; i--) {
			if (!UpdateFileEntries[i]->need_to_update)
				continue;
			for (int j = i - 1; j >= 0; j--) {
				if (UpdateFileEntries[j]->need_to_update && UpdateFileEntries[i]->location_id == UpdateFileEntries[j]->location_id && strcmp(UpdateFileEntries[i]->local_name, UpdateFileEntries[j]->local_name) == 0) {
					UpdateFileEntries[j]->need_to_update = 0;
				}
			}
			if (UpdateFileEntries[i]->need_to_update) {

				wchar_t existingfilepath[1024 + 260] = L"";

				wchar_t* existingfpdir = dir_temp_h2;
				if (UpdateFileEntries[i]->location_id == 1) {
					existingfpdir = H2ProcessFilePath;
				}
				else if (UpdateFileEntries[i]->location_id == 2) {
					existingfpdir = H2AppDataLocal;
				}

				swprintf(existingfilepath, 1024 + 260, L"%ws%hs", existingfpdir, UpdateFileEntries[i]->local_name);

				DWORD crc32_file = 0;
				if (ComputeFileCrc32Hash(existingfilepath, crc32_file)) {
					if (UpdateFileEntries[i]->crc32 == crc32_file) {
						UpdateFileEntries[i]->need_to_update = 0;
						continue;
					}
				}

				if (UpdateFileEntries[i]->location_id) {
					swprintf(existingfilepath, 1024 + 260, L"%ws%hs\\%hs", dir_update, H2UpdateLocationsStr[UpdateFileEntries[i]->location_id], UpdateFileEntries[i]->local_name);
				}
				else {
					swprintf(existingfilepath, 1024 + 260, L"%ws%hs", dir_update, UpdateFileEntries[i]->local_name);
				}

				if (ComputeFileCrc32Hash(existingfilepath, crc32_file)) {
					if (UpdateFileEntries[i]->crc32 == crc32_file) {
						UpdateFileEntries[i]->need_to_update = 1;
						continue;
					}
				}

			}
		}
		addDebugText("Filtered & locally checked the Download List.");
	}


	//prompt user with changes

	entry_count = UpdateFileEntries.size();
	for (int i = 0; i < entry_count; i++) {
		if (UpdateFileEntries[i]->need_to_update == 2) {
			GSDownload_files_to_download = true;
		}
		if (UpdateFileEntries[i]->need_to_update == 1 && UpdateFileEntries[i]->location_id > 0) {
			GSDownload_files_to_install = true;
		}
		if (GSDownload_files_to_download && GSDownload_files_to_install)
			break;
	}

	std::string im_lazy = "Download the Following:\n";

	for (int i = 0; i < entry_count; i++) {
		if (UpdateFileEntries[i]->need_to_update == 2) {
			std::string im_lazy2 = UpdateFileEntries[i]->server_uri;
			im_lazy += im_lazy2 + "\n";
		}
	}

	std::string im_lazy1 = "Install the Following:\n";
	im_lazy += im_lazy1;

	for (int i = 0; i < entry_count; i++) {
		if (UpdateFileEntries[i]->need_to_update == 1 && UpdateFileEntries[i]->location_id > 0) {
			std::string im_lazy2 = UpdateFileEntries[i]->local_name;
			std::string im_lazy3 = H2UpdateLocationsStr[UpdateFileEntries[i]->location_id];
			im_lazy += im_lazy3 + "\\" + im_lazy2 + "\n";
		}
	}

	extern char* Auto_Update_Text;
	char* Auto_Update_Text_alt = Auto_Update_Text;
	Auto_Update_Text = 0;
	if (Auto_Update_Text_alt) {
		free(Auto_Update_Text_alt);
	}
	Auto_Update_Text_alt = (char*)malloc(im_lazy.size() + 1);
	strcpy_s(Auto_Update_Text_alt, im_lazy.size() + 1, im_lazy.c_str());
	Auto_Update_Text = Auto_Update_Text_alt;
}

bool DownloadUpdatedFiles() {
	
	//download list
	bool files_downloaded = false;

	wchar_t* dir_temp = _wgetenv(L"TEMP");

	wchar_t dir_update[1024];
	swprintf(dir_update, 1024, L"%ws\\Halo2\\Update\\", dir_temp);

	int entry_count = UpdateFileEntries.size();
	for (int i = 0; i < entry_count; i++) {
		if (UpdateFileEntries[i]->need_to_update == 2) {
			files_downloaded = true;
			wchar_t existingfilepath[1024 + 260] = L"";
			if (UpdateFileEntries[i]->location_id) {
				swprintf(existingfilepath, 1024 + 260, L"%s%hs\\%hs", dir_update, H2UpdateLocationsStr[UpdateFileEntries[i]->location_id], UpdateFileEntries[i]->local_name);
			}
			else {
				swprintf(existingfilepath, 1024 + 260, L"%s%hs", dir_update, UpdateFileEntries[i]->local_name);
			}
			_wremove(existingfilepath);
			std::string im_lazy_2_dl = "https://cartographer.online/";
			im_lazy_2_dl += UpdateFileEntries[i]->server_uri;
			DownloadFile(im_lazy_2_dl.c_str(), existingfilepath);
		}
	}

	return files_downloaded;
}

static HANDLE hThreadDownloader = 0;

static DWORD WINAPI DownloadThread(LPVOID lParam)
{
	int operation_id = (int)lParam;

	setButtonState(1, 0);
	setButtonState(2, 0);
	setButtonState(3, 0);

	if (operation_id == 0) {
		setButtonState(1, 2);
		FetchUpdateDetails();
	}
	else if (operation_id == 1) {
		setButtonState(2, 2);
		if (DownloadUpdatedFiles())
			FetchUpdateDetails();
	}

	setButtonState(1, 1);
	if (GSDownload_files_to_download)
		setButtonState(2, 1);
	else
		setButtonState(2, 0);
	if (GSDownload_files_to_install)
		setButtonState(3, 1);
	else
		setButtonState(3, 0);
	
	hThreadDownloader = 0;
	return 0;
}

void GSDownloadInit() {
	setButtonState(1, 1);
}

void GSDownloadCheck() {
	if (!hThreadDownloader)
		hThreadDownloader = CreateThread(NULL, 0, DownloadThread, (LPVOID)0, 0, NULL);
}

void GSDownloadDL() {
	if (!hThreadDownloader)
		hThreadDownloader = CreateThread(NULL, 0, DownloadThread, (LPVOID)1, 0, NULL);
}

void GSDownloadInstall() {

	setButtonState(3, 2);

	wchar_t* dir_temp = _wgetenv(L"TEMP");

	wchar_t dir_update[1024];
	swprintf(dir_update, 1024, L"%ws\\Halo2\\Update\\", dir_temp);


	std::string updater_params = "";

	int entry_count = UpdateFileEntries.size();
	for (int i = 0; i < entry_count; i++) {
		if (UpdateFileEntries[i]->need_to_update == 1 && UpdateFileEntries[i]->location_id > 0) {
			updater_params += "\"";
			updater_params += H2UpdateLocationsStr[UpdateFileEntries[i]->location_id];
			updater_params += "\\";
			updater_params += UpdateFileEntries[i]->local_name;
			updater_params += "\" \"";

			char existingfilepath[1024 + 260] = "";

			wchar_t* existingfpdir = 0;
			if (UpdateFileEntries[i]->location_id == 1) {
				existingfpdir = H2ProcessFilePath;
			}
			else if (UpdateFileEntries[i]->location_id == 2) {
				existingfpdir = H2AppDataLocal;
			}

			snprintf(existingfilepath, 1024 + 260, "%ws%hs", existingfpdir, UpdateFileEntries[i]->local_name);

			updater_params += existingfilepath;

			updater_params += "\" ";
		}
	}

	//exec installer
	//quit game

	char existingfilepathupdater[1024 + 260] = "";
	snprintf(existingfilepathupdater, 1024 + 260, "%wsh2pc-update.exe", dir_update);

	if (updater_params.size() > 0) {
		int updater_params_buflen = 20 + updater_params.size();
		char* updater_params_flags = (char*)malloc(sizeof(char) * updater_params_buflen);
		snprintf(updater_params_flags, updater_params_buflen, "-p %d -t 5000 %s", GetCurrentProcessId(), updater_params.c_str());
		if (fork_cmd_elevate(existingfilepathupdater, updater_params_flags)) {
			addDebugText("Shutting down to update!");
			BYTE& Quit_Exit_Game = *(BYTE*)((char*)H2BaseAddr + (H2IsDediServer ? 0x4a7083 : 0x48220b));
			Quit_Exit_Game = 1;
		}
		else {
			addDebugText("Failed to fork updater!");
			addDebugText(existingfilepathupdater);
			add_cartographer_label(CMLabelMenuId_Update, 3, H2CustomLanguageGetLabel(CMLabelMenuId_Update, 0xFFFFF003), true);
		}
		free(updater_params_flags);
	}
	else {
		addDebugText("No Files to Update!");
	}

}

void GSDownloadCancel() {
	//DWORD result = WaitForSingleObject(hThreadDownloader, 0);
	//if (result == WAIT_OBJECT_0) {
		// the thread handle is signaled - the thread has terminated
	//}
	//else {
		// the thread handle is not signaled - the thread is still alive
	//}
	if (hThreadDownloader) {
		TerminateThread(hThreadDownloader, 1);
		hThreadDownloader = 0;
		Size_Of_Download = Size_Of_Downloaded = 0;
	}

	extern char* Auto_Update_Text;
	if (Auto_Update_Text) {
		free(Auto_Update_Text);
		Auto_Update_Text = 0;
	}

	GSDownload_files_to_download = false;
	GSDownload_files_to_install = false;

	setButtonState(1, 0);
	setButtonState(2, 0);
	setButtonState(3, 0);
}


void InitGSDownload() {

}

void DeinitGSDownload() {

}