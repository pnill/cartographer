#include "stdafx.h"

#include "curl-interface.h"

#ifdef _DEBUG
#pragma comment(lib, "libcurl_a_debug.lib")
#else
#pragma comment(lib, "libcurl_a.lib")
#endif

CURL* curl_interface_init_no_ssl()
{
	CURL* curl = curl_easy_init();
	if (curl != nullptr)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		return curl;
	}

	return nullptr;
}