#include "stdafx.h"

#include "H2MOD.h"
#include "H2MOD\GUI\GUI.h"
#include "H2MOD\Modules\Input\ControllerInput.h"
#include "H2MOD\Modules\Input\KeyboardInput.h"
#include "H2MOD\Modules\Input\Mouseinput.h"
#include "H2MOD\Modules\Input\PlayerControl.h"
#include "H2MOD\Modules\Shell\Startup\Startup.h"
#include "H2MOD\Modules\UI\XboxLiveTaskProgress.h"
#include "imgui_handler.h"
#include "Util\Hooks\Hook.h"
#include "XLive\xnet\IpManagement\XnIp.h"

extern int notify_xlive_ui;
namespace ImGuiHandler
{
	namespace ImMOTD {
		namespace
		{
			bool g_motd = false;
			bool g_complete = false;
			bool g_success = false;	
			bool g_init = false;
			std::atomic<bool> g_textureLoading = false;
			int X;
			int Y;
		}
		bool GetMOTD(s_aspect_ratio ratio)
		{
			// check if the texture has been reset
			if (g_complete 
				&& ImGuiHandler::GetTexture(patch_notes) != nullptr)
				return g_success;

			auto path = std::wstring(H2AppDataLocal);
			auto cpath = std::string(path.begin(), path.end()) + "motd.png";

			// if we got this far, and g_complete is true, we already downloaded the image from the webserver
			// and the texture got reset, so we just re-load it
			if (g_complete)
			{
				ImGuiHandler::LoadTextureFromFile(cpath.c_str(), patch_notes, &X, &Y);
				return g_success;
			}

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

			curl = curl_interface_init_no_verify();
			if (curl)
			{
				fp = fopen(cpath.c_str(), "wb");
				curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				fseek(fp, 0, SEEK_END);
				int size = ftell(fp);
				fclose(fp);
				if (size > 10252) {
					ImGuiHandler::LoadTextureFromFile(cpath.c_str(), patch_notes, &X, &Y);
					g_success = true;
				}
			}
			g_complete = true;
			return g_success;
		}
		void Render(bool *p_open)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			s_aspect_ratio ratio = getAspectRatio(viewport->WorkSize);

			// check if the texture has been reset
			if (ImGuiHandler::GetTexture(patch_notes) == nullptr
				&& !g_textureLoading)
			{
				// if so, re-upload it to memory
				g_init = false;
				g_motd = false;
			}

			if(!g_init)
			{
				if (!g_motd)
				{
					g_textureLoading = true;
					auto grab_thread = []()
					{
						GetMOTD(getAspectRatio(ImGui::GetMainViewport()->WorkSize));

						g_textureLoading = false;
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
							ImMOTD::Close();
						}
						g_init = true;
					}
				}
			}
			else {
				ImDrawList* foreground_draw_list = ImGui::GetForegroundDrawList();
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
				foreground_draw_list->AddImage((void*)ImGuiHandler::GetTexture(patch_notes), ImVec2(0, 0), ImGui::GetIO().DisplaySize);
				/*draw_list->AddImage((void*)imgui_handler::GetImage(patch_notes), ImVec2(0, 0),
					ImVec2(X, Y));*/

				if (ControllerInput::get_controller_input(0)[16] == 1)
				{
					*p_open = false;
					ImMOTD::Close();
					notify_xlive_ui = 0;
				}
				else if (MouseInput::GetMouseState()[12] != 0)
				{
					*p_open = false;
					ImMOTD::Close();
					notify_xlive_ui = 0;
				}
				else
				{
					BYTE bKeys[256];
					GetKeyboardState(bKeys);
					for (int i = 0; i < ARRAYSIZE(bKeys); i++)
					{
						if (bKeys[i] & 0x80)
						{
							*p_open = false;
							ImMOTD::Close();
							notify_xlive_ui = 0;
							break;
						}
					}
				}

			}
		}
		void Open()
		{

		}
		void Close()
		{
			XUserSignInSetStatusChanged(0);
		}
	}
}
