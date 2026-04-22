#include "stdafx.h"
#include "curl-interface.h"


CURL* curl_interface_init()
{
	return curl_easy_init();
}

CURL* curl_interface_init_no_verify()
{
	CURL* curl = curl_interface_init();
	if (curl != nullptr)
	{

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Cartographer " DLL_VERSION_STR);

		return curl;
	}

	return nullptr;
}