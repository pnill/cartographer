#include "stdafx.h"

#include "imgui_handler.h"

#include "interface/screens/screen_xbox_live_task_progress_dialog.h"

#include "H2MOD/Modules/Input/ControllerInput.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Modules/Input/Mouseinput.h"
#include "H2MOD/Modules/Input/PlayerControl.h"
#include "H2MOD/Modules/Shell/Startup/Startup.h"

#include "XLive/xnet/IpManagement/XnIp.h"

extern int notify_xlive_ui;

namespace ImGuiHandler
{
	namespace ImMOTD {
		std::string windowName = "motd";

		namespace
		{
			bool load_complete = false;
			bool download_success = false;
			bool download_complete = false;
			std::atomic<bool> motd_texture_load_in_progress = false;
			int X;
			int Y;
		}
		bool DownloadMOTD(const std::wstring& file_path, s_aspect_ratio ratio)
		{
			FILE* fp = NULL;
			CURL* curl;
			CURLcode res;
			std::string url = "http://www.halo2pc.com/motd.png";

			bool success = false;

			curl = curl_interface_init_no_verify();
			if (curl)
			{
				fp = _wfopen(file_path.c_str(), L"wb");
				curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				res = curl_easy_perform(curl);
				if (res == CURLcode::CURLE_OK)
				{
					fseek(fp, 0, SEEK_END);
					int size = ftell(fp);
					if (size > 10252) 
					{
						success = true;
					}
					fclose(fp);
				}
				else
				{
					fclose(fp);
					// _wremove(file_path.c_str());
				}
				curl_easy_cleanup(curl);
			}

			return success;
		}
		bool LoadMOTD(const std::wstring& file_path, s_aspect_ratio ratio)
		{
			if (std::filesystem::exists(file_path))
			{
				return ImGuiHandler::LoadTextureFromFile(file_path.c_str(), patch_notes, &X, &Y);
			}
			return false;
		}
		void DownloadAndLoadMOTD()
		{
			motd_texture_load_in_progress = true;
			std::wstring motd_path_wide = std::wstring(H2AppDataLocal) + L"motd.png";
			if (!download_complete)
			{
				download_success = DownloadMOTD(motd_path_wide, GetAspectRatio(ImGui::GetMainViewport()->WorkSize));
				download_complete = true;
			}
			load_complete = LoadMOTD(motd_path_wide, GetAspectRatio(ImGui::GetMainViewport()->WorkSize));
			motd_texture_load_in_progress = false;
		}

		void Render(bool* p_open)
		{
			if (!download_complete
				|| motd_texture_load_in_progress)
				return;

			// if MOTD cannot be loaded, just close the menu
			if (!load_complete)
			{
				ImGuiHandler::ToggleWindow(ImGuiHandler::ImMOTD::windowName);
				return;
			}

			if (ImGuiHandler::GetTexture(patch_notes) == nullptr)
			{
				load_complete = false;
				std::thread(DownloadAndLoadMOTD).detach();
				return;
			}

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

			ImGuiWindowFlags window_flags = 0
				| ImGuiWindowFlags_NoDecoration
				| ImGuiWindowFlags_NoSavedSettings
				| ImGuiWindowFlags_NoBackground
				;

			ImGuiIO& io = ImGui::GetIO();
			const ImGuiViewport* viewport = ImGui::GetMainViewport();

			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			if (ImGui::Begin("##motd", NULL, window_flags))
			{
				ImTextureID texId = ImGuiHandler::GetTexture(patch_notes);
				ImGui::Image(texId, ImGui::GetIO().DisplaySize);

				if (ControllerInput::get_controller_input(0)[16] == 1
					|| ImGui::IsItemClicked())
				{
					ImGuiHandler::ToggleWindow(ImGuiHandler::ImMOTD::windowName);
				}
				else
				{
					for (int i = 0; i < ARRAYSIZE(io.KeysDown); i++)
					{
						if (ImGui::IsKeyPressed(i))
						{
							ImGuiHandler::ToggleWindow(ImGuiHandler::ImMOTD::windowName);
							break;
						}
					}
				}
			}
			ImGui::PopStyleVar(2);
			ImGui::End();
		}
		void Open()
		{
			if (!download_complete)
				std::thread(DownloadAndLoadMOTD).detach();
		}
		void Close()
		{
			notify_xlive_ui = 0;
			XUserSignInSetStatusChanged(0);
		}
	}
}
