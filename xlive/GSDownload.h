#pragma once

void InitGSDownload();
void DeinitGSDownload();

void GSDownloadCheck();
void GSDownloadDL();
void GSDownloadInstall();
void GSDownloadCancel();

extern bool GSDownload_files_to_download;
extern bool GSDownload_files_to_install;
