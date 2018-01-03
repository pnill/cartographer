#pragma once

void InitGSDownload();
void DeinitGSDownload();

void GSDownloadInit();
void GSDownloadCheck();
void GSDownloadDL();
void GSDownloadInstall();
void GSDownloadCancel();

extern bool GSDownload_files_to_download;
extern bool GSDownload_files_to_install;
extern long long Size_Of_Download;
extern long long Size_Of_Downloaded;
