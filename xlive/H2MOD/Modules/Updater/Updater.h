#pragma once

void InitGSDownload();
void DeinitGSDownload();

void GSDownloadInit();
void GSDownloadCheck();
void GSDownloadDL();
void GSDownloadInstall();
void GSDownloadCancel();

extern bool updater_has_files_to_download;
extern bool updater_has_files_to_install;
extern long long sizeOfDownload;
extern long long sizeOfDownloaded;
