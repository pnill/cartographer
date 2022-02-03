#pragma once

#include "imgui.h"

namespace imgui
{
	namespace Cartographer
	{
		void StyleColorsMaik(ImGuiStyle* dst = nullptr);
		void StyleSetAsDefault(ImGuiStyle* dst = nullptr);
		void PushStyle();
		void PopStyle();
	}
}
