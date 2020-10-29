#include "imgui_handler.h"
#include "H2MOD/GUI/GUI.h"
#include "H2MOD.h"
#include <curl/curl.h>

namespace imgui_handler
{
	namespace
	{
		struct curl_response_text {
			char *ptr;
			size_t len;
		};

		static void init_curl_response(struct curl_response_text *s) {
			s->len = 0;
			s->ptr = (char*)malloc(s->len + 1);
			if (s->ptr == NULL) {
				fprintf(stderr, "malloc() failed\n");
				exit(EXIT_FAILURE);
			}
			s->ptr[0] = '\0';
		}

		static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct curl_response_text *s)
		{
			size_t new_len = s->len + size * nmemb;
			s->ptr = (char*)realloc(s->ptr, new_len + 1);
			if (s->ptr == NULL) {
				fprintf(stderr, "realloc() failed\n");
				exit(EXIT_FAILURE);
			}
			memcpy(s->ptr + s->len, ptr, size*nmemb);
			s->ptr[new_len] = '\0';
			s->len = new_len;

			return size * nmemb;
		}
		bool b_motd_fetched = false;
		std::string motd = "Thank you for playing!\n please do not hack or cheat";
		std::string getMOTD()
		{
			if(!b_motd_fetched)
			{
				std::string http_request_body = "https://www.halo2pc.com/motd.txt";
				char* response;
				CURL *curl;
				CURLcode curlResult;
				CURLcode global_init = curl_global_init(CURL_GLOBAL_ALL);
				struct curl_response_text s;
				init_curl_response(&s);
				if(global_init != CURLE_OK)
				{
					b_motd_fetched = true;
					return motd;
				}
				curl = curl_easy_init();
				if(curl)
				{
					curl_easy_setopt(curl, CURLOPT_URL, http_request_body.c_str());
					curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
					curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
					curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
					curlResult = curl_easy_perform(curl);
					if (curlResult != CURLE_OK)
					{
						curl_easy_cleanup(curl);
						return motd;

					}
					else
					{
						curl_easy_cleanup(curl);
						motd = std::string(s.ptr);
						LOG_INFO_GAME(motd);
						b_motd_fetched = true;
						return motd;
					}
				}
			}
			else
			{
				return motd;
			}
		}
	}
	void MessageOfTheDay(bool *p_open)
	{
		ImGuiIO& io = ImGui::GetIO();

		RECT rect;
		::GetClientRect(get_HWND(), &rect);
		io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
		//ImGui::PushFont(font2);
		ImGui::SetNextWindowSize(ImVec2(650, 530), ImGuiCond_Appearing);
		ImGui::SetNextWindowSizeConstraints(ImVec2(610, 530), ImVec2(1920, 1080));
		if (h2mod->GetMapType() == MainMenu)
			ImGui::SetNextWindowBgAlpha(1);
		if (ImGui::Begin("   Message of The Day", p_open, window_flags))
		{
			ImGui::TextWrapped(getMOTD().c_str());
		}
	}
}
