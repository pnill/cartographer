#include "3rdparty/imgui/imgui.h"
#include "imgui_handler.h"
#include "Blam/Common/Common.h"
#include "H2MOD.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Input/PlayerControl.h"

namespace imgui_handler
{
	namespace iMessageBox
	{
		namespace
		{
			std::string message;
		}
		void Render(bool* p_open)
		{
			ImGuiIO& io = ImGui::GetIO();
			RECT rect;
			::GetClientRect(get_HWND(), &rect);
			io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_NoResize;
			//window_flags |= ImGuiWindowFlags_MenuBar;
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
			//ImGui::PushFont(font2);
			ImGui::SetNextWindowSize(ImVec2(650, 250), ImGuiCond_Appearing);
			ImGui::SetNextWindowSizeConstraints(ImVec2(610, 250), ImVec2(1920, 1080));
			if (h2mod->GetMapType() == MainMenu)
				ImGui::SetNextWindowBgAlpha(1);
			if (ImGui::Begin("Message", p_open, window_flags))
			{
				ImGui::TextWrapped(message.c_str());
				ImGui::SetCursorPosY(190);
				if(ImGui::Button("Ok", ImVec2(610, 50)))
				{
					*p_open = false;
					Close();
				}
			}
		}
		void SetMessage(std::string Message)
		{
			message = Message;
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
