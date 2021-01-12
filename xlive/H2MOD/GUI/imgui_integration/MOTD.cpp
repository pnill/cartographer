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
#include "H2MOD/Modules/Input/KeyboardInput.h"

extern int notify_xlive_ui;
namespace imgui_handler
{
	namespace MOTD {
		namespace
		{
			bool g_motd = false;
			bool g_complete = false;
			bool g_success = false;	
			bool g_init = false;
			int X;
			int Y;
		}
		bool GetMOTD(s_aspect_ratio ratio)
		{
			if (g_complete)
				return g_success;

			CURL *curl;
			FILE *fp;
			CURLcode res;
			std::string url = "http://www.halo2pc.com/motd.png";
			/*switch (ratio)
			{
				case four_three:
					url += "motd_43.png";
					break;
				case sixten_nine:
					url += "motd_169.png";
					break;
				default:
					return false;
			}*/
			auto path = std::wstring(H2AppDataLocal);
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
				if (size > 10252) {
					ImGuiIO& io = ImGui::GetIO();
					RECT rect;
					::GetClientRect(get_HWND(), &rect);
					io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
					//int X = ImGui::GetIO().DisplaySize.x;
					//int Y = ImGui::GetIO().DisplaySize.y;
					imgui_handler::LoadTextureFromFile(cpath.c_str(), patch_notes, &X, &Y);
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
				//ImVec2 Resolution(
				//	ImGui::GetIO().DisplaySize.x,
				//	ImGui::GetIO().DisplaySize.y
				//);
				//ImVec2 Dimensions;
				//switch(ratio)
				//{
				//	case four_three: 
				//		Dimensions = ImVec2(
				//			1184, 745
				//		);
				//	break;
				//	default:
				//	case sixten_nine:
				//		Dimensions = ImVec2(
				//			1786, 745
				//		);
				//	break;
				//
				//}

				//float MinScale = 1.5f;

				//ImVec2 Scale(
				//	fmaxf(MinScale, (Dimensions.x / Resolution.x)),
				//	fmaxf(MinScale, (Dimensions.y / Resolution.y))
				//);
				///*Scale.x = fmaxf(MinScale, Scale.x);
				//Scale.y = fmaxf(MinScale, Scale.y);*/
				//float scaledx = Dimensions.x / Scale.x;
				//float scaledy = Dimensions.y / Scale.y;

				//ImVec2 TopLeft(
				//	(ImGui::GetIO().DisplaySize.x / 2) - (scaledx / 2),
				//	(ImGui::GetIO().DisplaySize.y / 2) - (scaledy / 2)
				//);
				//ImVec2 BottomRight(
				//	(ImGui::GetIO().DisplaySize.x / 2) + (scaledx / 2),
				//	(ImGui::GetIO().DisplaySize.y / 2) + (scaledy / 2)
				//);
				draw_list->AddImage((void*)imgui_handler::GetImage(patch_notes), ImVec2(0,0), 
					ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
				/*draw_list->AddImage((void*)imgui_handler::GetImage(patch_notes), ImVec2(0, 0),
					ImVec2(X, Y));*/

				if (ControllerInput::get_controller_input(0)[16] == 1)
				{
					*p_open = false;
					MOTD::Close();
					notify_xlive_ui = 0;
				}
				if (MouseInput::GetMouseState()[0xC] != 0)
				{
					*p_open = false;
					MOTD::Close();
					notify_xlive_ui = 0;
				}
				BYTE bKeys[256];
				GetKeyboardState(bKeys);
				for (unsigned char bKey : bKeys)
				{
					if(bKey & 0x80)
					{
						*p_open = false;
						MOTD::Close();
						notify_xlive_ui = 0;
						break;
					}
				}
			}
		}
		void Open()
		{
			WriteValue<byte>(h2mod->GetAddress(0x9712cC), 1);
			ImGuiToggleInput(true);
			PlayerControl::GetControls(0)->DisableCamera = true;
		}
		void Close()
		{
			WriteValue<byte>(h2mod->GetAddress(0x9712cC), 0);
			ImGuiToggleInput(false);
			PlayerControl::GetControls(0)->DisableCamera = false;
		}
	}
}
