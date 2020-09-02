#include "StatsHandler.h"
#include "stdafx.h"
#include "Globals.h"
#include "Util/hash.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "libcurl/curl/curl.h"
#include "H2MOD/Modules/Console/ConsoleCommands.h"
#ifdef _DEBUG
#pragma comment(lib, "libcurl_a_debug.lib")
#else
#pragma comment(lib, "libcurl_a.lib")
#endif

StatsHandler::StatsHandler()
= default;
struct stringMe {
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

void StatsHandler::sendStats()
{
	commands->display("Cream Cheese is pretty meh");
	//int verifyPlaylistResponse = verifyPlaylist();
	//if(verifyPlaylistResponse == 500)
	//{
	//	LOG_ERROR_GAME(L"[H2MOD] Playlist Verification encountered a server error");
	//}
	//else if(verifyPlaylistResponse == 201)
	//{
	//	//Upload playlist
	//}
}

int StatsHandler::uploadPlaylist()
{
	char http_request_body[] = "";

	return -1;
}

void TestMe(bool a)
{
	
}

int StatsHandler::verifyPlaylist()
{
	std::string http_request_body = "https://www.halo2pc.com/test-pages/CartoStat/API/get.php?Type=PlaylistCheck&Playlist_Checksum=";
	wchar_t* playlist_file = h2mod->GetAddress<wchar_t*>(0, 0x46ECC4);
#if _DEBUG
	LOG_INFO_GAME(L"[H2MOD] playlistVerified loaded playlist is {0}", playlist_file);
#endif
	std::string checksum;
	if (!hashes::calc_file_md5(playlist_file, checksum))
	{
		LOG_ERROR_GAME(L"[H2MOD] playlistVerified failed to Hash Playlist file");
		return -1;
	}
	else
	{
#if _DEBUG
		LOG_INFO_GAME(checksum);
#endif		
		http_request_body.append(checksum);
#if _DEBUG
		LOG_INFO_GAME(http_request_body);
#endif
		char* Response;
		CURL *curl;
		CURLcode curlResult;
		CURLcode global_init = curl_global_init(CURL_GLOBAL_ALL);
		if(global_init != CURLE_OK)
		{
			LOG_INFO_GAME(L"[H2MOD]::playlistVerified failed to init curl");
			return -1;
		}
		curl = curl_easy_init();
		if(curl)
		{
			//Set the URL for the GET
			curl_easy_setopt(curl, CURLOPT_URL, http_request_body.c_str());
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curlResult = curl_easy_perform(curl);
			if(curlResult != CURLE_OK)
			{
				//LOG_INFO_GAME(L"[H2MOD]::playlistVerified failed to execute curl - {0}", curl_easy_strerror(curlResult));
				return -1;
			} else
			{
				int response_code;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
#if _DEBUG
				LOG_INFO_GAME(std::to_string(response_code));
#endif
				return response_code;
			}
			curl_easy_cleanup(curl);
		}
	}
	return -1;
}
