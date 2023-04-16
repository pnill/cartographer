#pragma once

#include "curl/curl.h"

// curl errors
#define ERROR_CODE_CURL_SOCKET_FAILED -40
#define ERROR_CODE_CURL_HANDLE -41
#define ERROR_CODE_CURL_EASY_PERF -42

CURL* curl_interface_init();
CURL* curl_interface_init_no_verify();