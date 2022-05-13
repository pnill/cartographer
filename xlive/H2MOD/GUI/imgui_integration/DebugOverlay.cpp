#include "stdafx.h"

#include "imgui_handler.h"
#include "imgui_internal.h"

namespace ImGuiHandler
{
	namespace ImDebugOverlay
	{
		struct DebugWatchItem
		{
			std::string Description;
			std::string Value;
		};
		std::unordered_map<std::string, DebugWatchItem> WatchItems;
		void Render(bool* p_open)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			s_aspect_ratio ratio = getAspectRatio(viewport->WorkSize);

			ImDrawList* draw_list = ImGui::GetForegroundDrawList();

			//draw_list->AddText(ImGui::GetDefaultFont(), 20, ImVec2(50, 200), ImColor(0, 0, 0), "Banana");
			int index = 0;
			for(auto item : WatchItems)
			{
				std::string temp_str = item.second.Description;
				temp_str += ": ";
				temp_str += item.second.Value;
				draw_list->AddText(ImGui::GetDefaultFont(), 20, ImVec2(51, 201 + (20 * index + 5)), ImColor(0, 0, 0), temp_str.c_str());
				draw_list->AddText(ImGui::GetDefaultFont(), 20, ImVec2(50, 200 + (20 * index + 5)), ImColor(255, 255, 255), temp_str.c_str());
				index++;
			}
		}
		void AddWatchItem(std::string Key, std::string Description)
		{
			WatchItems.emplace(Key, DebugWatchItem{ Description, "" });
		}
		void UpdateWatchItem(std::string Key, std::string Value)
		{
			if (WatchItems.count(Key))
				WatchItems.at(Key).Value = Value;
		}
		void Open()
		{
			
		}
		void Close()
		{
			
		}
	}
}
