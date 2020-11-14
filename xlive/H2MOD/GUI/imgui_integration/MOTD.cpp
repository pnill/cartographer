#include "imgui_handler.h"
#include "H2MOD/GUI/GUI.h"
#include "H2MOD.h"
#include <curl/curl.h>
#include "H2MOD/Modules/Input/ControllerInput.h"
#include "H2MOD/Modules/Input/PlayerControl.h"
#include "XLive\xnet\IpManagement\XnIp.h"
#include "H2MOD\Modules\UI\XboxLiveTaskProgress.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Input/Mouseinput.h"
#include "H2MOD/Modules/Startup/Startup.h"

extern int notify_xlive_ui;
namespace imgui_handler
{
	namespace MOTD {
		namespace
		{
			int DimX = 1786;
			int DimY = 745;
			/*struct curl_response_text {
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
			}*/
			/*std::string motd = "Thank you for playing!\n please do not hack or cheat";
			std::string getMOTD()
			{
				if (!b_motd_fetched)
				{
					std::string http_request_body = "https://www.halo2pc.com/motd.txt";
					char* response;
					CURL *curl;
					CURLcode curlResult;
					CURLcode global_init = curl_global_init(CURL_GLOBAL_ALL);
					struct curl_response_text s;
					init_curl_response(&s);
					if (global_init != CURLE_OK)
					{
						b_motd_fetched = true;
						return motd;
					}
					curl = curl_easy_init();
					if (curl)
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
			}*/
			bool g_motd = false;
			bool g_complete = false;
			bool g_success = false;	
			bool g_init = false;
		}
		bool GetMOTD(s_aspect_ratio ratio)
		{
			if (g_complete)
				return g_success;

			CURL *curl;
			FILE *fp;
			CURLcode res;
			std::string url = "http://www.halo2pc.com/";
			switch (ratio)
			{
				case four_three:
					url += "motd_43.png";
					break;
				case sixten_nine:
					url += "motd_169.png";
					break;
				default:
					return false;
			}
			auto path = std::wstring(H2ProcessFilePath);
			auto cpath = std::string(path.begin(), path.end()) + "motd.png";

			curl = curl_easy_init();
			if (curl)
			{
				fp = fopen(cpath.c_str(), "wb");
				curl_easy_setopt(curl, CURLOPT_URL, url);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				fseek(fp, 0, SEEK_END);
				int size = ftell(fp);
				fclose(fp);
				if (size > 5126) {
					imgui_handler::LoadTextureFromFile(cpath.c_str(), patch_notes, &DimX, &DimY);
					g_success = true;
				}
			}
			g_complete = true;
			return g_success;
		}
		void Render(bool *p_open)
		{

			ImGuiIO& io = ImGui::GetIO();
			RECT rect;
			::GetClientRect(get_HWND(), &rect);
			io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
			s_aspect_ratio ratio = getAspectRatio(
				ImGui::GetIO().DisplaySize.x,
				ImGui::GetIO().DisplaySize.y);
			if(!g_init)
			{
				if(!g_motd)
				{
					auto grab_thread = []()
					{
						GetMOTD(getAspectRatio(
							ImGui::GetIO().DisplaySize.x,
							ImGui::GetIO().DisplaySize.y));
					};
					std::thread(grab_thread).detach();
					g_motd = true;
				} 
				else
				{
					if(g_complete)
					{
						if (!g_success) {
							*p_open = false;
							notify_xlive_ui = 0;
							MOTD::Close();
						}
						g_init = true;
					}
				}
			}
			else {
				ImDrawList* draw_list = ImGui::GetOverlayDrawList();
				ImVec2 Resolution(
					ImGui::GetIO().DisplaySize.x,
					ImGui::GetIO().DisplaySize.y
				);
				ImVec2 Dimensions;
				switch(ratio)
				{
					case four_three: 
						Dimensions = ImVec2(
							1184, 745
						);
					break;
					default:
					case sixten_nine:
						Dimensions = ImVec2(
							1786, 745
						);
					break;
				
				}

				float MinScale = 1.5f;

				ImVec2 Scale(
					fmaxf(MinScale, (Dimensions.x / Resolution.x)),
					fmaxf(MinScale, (Dimensions.y / Resolution.y))
				);
				/*Scale.x = fmaxf(MinScale, Scale.x);
				Scale.y = fmaxf(MinScale, Scale.y);*/
				float scaledx = Dimensions.x / Scale.x;
				float scaledy = Dimensions.y / Scale.y;

				ImVec2 TopLeft(
					(ImGui::GetIO().DisplaySize.x / 2) - (scaledx / 2),
					(ImGui::GetIO().DisplaySize.y / 2) - (scaledy / 2)
				);
				ImVec2 BottomRight(
					(ImGui::GetIO().DisplaySize.x / 2) + (scaledx / 2),
					(ImGui::GetIO().DisplaySize.y / 2) + (scaledy / 2)
				);
				draw_list->AddImage((void*)imgui_handler::GetImage(patch_notes), TopLeft, BottomRight);

				if (ControllerInput::get_controller_input(0)[16] == 1)
				{
					*p_open = false;
					MOTD::Close();
					notify_xlive_ui = 0;
				}
				if(MouseInput::GetMouseState()[0xC] != 0)
				{
					*p_open = false;
					MOTD::Close();
					notify_xlive_ui = 0;
				}

				//ImGuiIO& io = ImGui::GetIO();
				//RECT rect;
				//::GetClientRect(get_HWND(), &rect);
				//io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
				//ImGuiWindowFlags window_flags = 0;
				//window_flags |= ImGuiWindowFlags_NoCollapse;
				//window_flags |= ImGuiWindowFlags_NoResize;
				//window_flags |= ImGuiWindowFlags_NoMove;
				//ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
				////ImGui::PushFont(font2);
				//ImGui::SetNextWindowSize(ImVec2(650, 530), ImGuiCond_Appearing);
				//ImGui::SetNextWindowSizeConstraints(ImVec2(610, 530), ImVec2(1920, 1080));
				//if (h2mod->GetMapType() == MainMenu)
				//	ImGui::SetNextWindowBgAlpha(1);
				/*if (ImGui::Begin("   Message of the Day", p_open, window_flags))
				{
					ImGui::Image((void*)imgui_handler::GetImage(patch_notes), ImVec2(1786, 745));

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
					std::istringstream f(getMOTD());
					std::string s;
					while (std::getline(f, s, '\n'))
					{
						ImGui::TextWrapped(s.c_str());
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 0.5f);
					}
					ImGui::NewLine();
					ImGui::NewLine();
					ImGui::SetCursorPos({ 10, 480 + ImGui::GetScrollY() });
					if (ImGui::Button("Close", { 630, 40 }))
					{
						*p_open = false;
					}
					if (ControllerInput::get_controller_input(0)[16] == 1)
					{
						*p_open = false;
					}
				}*/
			}
		}
		void Open()
		{
			WriteValue<byte>(h2mod->GetAddress(0x9712cC), 1);
			PlayerControl::GetControls(0)->DisableCamera = true;
		}
		void Close()
		{
			WriteValue<byte>(h2mod->GetAddress(0x9712cC), 0);
			PlayerControl::GetControls(0)->DisableCamera = false;

		}
	}
}
