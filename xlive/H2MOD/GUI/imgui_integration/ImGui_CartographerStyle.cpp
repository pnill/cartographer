#include "stdafx.h"

#include "ImGui_CartographerStyle.h"

const static ImVec2 wPadding(20, 10);
const static ImVec2 fPadding(8, 3);
const static ImVec2 iSpacing(6, 6);
const static ImVec2 iiSpacing(6, 6);
const static float indentSpacing = 20.0f;
const static float ScrollBarSize = 16.0f;
const static float GrabMinSize = 13.0f;

const static float WindowBorderSize = 0.0f;
const static float ChildBorderSize = 0.0f;
const static float PopupBorderSize = 0.0f;
const static float FrameBorderSize = 0.0f;
const static float TabBorderSize = 0.0f;

const static float WindowRound = 0.0f;
const static float ChildRounding = 0.0f;
const static float FrameRounding = 0.0f;
const static float PopupRounding = 0.0f;
const static float ScrollbarRounding = 0.0f;
const static float GrabRounding = 0.0f;
const static float TabRounding = 0.0f;

const static ImVec2 WindowTitleAlign(0.0f, 0.50f);
const static ImVec2 ButtonTextAlign(0.50f, 0.50f);
const static ImVec2 SelectabletextAlign(0.0f, 0.50f);

void imgui::Cartographer::StyleColorsMaik(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst != nullptr ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text] = ImVec4(0.76f, 0.89f, 1.0f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.20f, 0.27f, 0.35f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.09f, 0.17f, 0.81f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.03f, 0.06f, 0.12f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.03f, 0.06f, 0.12f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.22f, 0.56f, 0.87f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.09f, 0.14f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.14f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.21f, 0.27f, 0.36f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.03f, 0.06f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.31f, 0.53f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.03f, 0.06f, 0.12f, 0.71f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.16f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.03f, 0.06f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.14f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.14f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.21f, 0.27f, 0.36f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.56f, 0.69f, 0.81f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.37f, 0.49f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.22f, 0.56f, 0.87f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.09f, 0.31f, 0.53f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.44f, 0.65f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.45f, 0.72f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.21f, 0.27f, 0.36f, 0.27f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.14f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.21f, 0.27f, 0.36f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.11f, 0.33f, 0.55f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.18f, 0.45f, 0.72f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.24f, 0.60f, 0.95f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.09f, 0.14f, 0.22f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.14f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.21f, 0.27f, 0.36f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.09f, 0.31f, 0.53f, 0.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.09f, 0.14f, 0.22f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.11f, 0.33f, 0.55f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.03f, 0.06f, 0.12f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.09f, 0.14f, 0.22f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.56f, 0.69f, 0.81f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.56f, 0.69f, 0.81f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.09f, 0.31f, 0.53f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.56f, 0.69f, 0.81f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.24f, 0.60f, 0.95f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.20f, 0.32f, 1.00f);
}

void imgui::Cartographer::StyleSetAsDefault(ImGuiStyle* out)
{
	// initialize default window style for each window
	auto& style = out != nullptr ? *out : ImGui::GetStyle();

	style.WindowPadding = wPadding;
	style.FramePadding = fPadding;
	style.ItemSpacing = iSpacing;
	style.ItemInnerSpacing = iiSpacing;
	style.IndentSpacing = indentSpacing;

	style.ScrollbarSize = ScrollBarSize;
	style.GrabMinSize = GrabMinSize;

	style.WindowBorderSize = WindowBorderSize;
	style.ChildBorderSize = ChildBorderSize;
	style.PopupBorderSize = PopupBorderSize;
	style.FrameBorderSize = FrameBorderSize;

	style.WindowRounding = WindowRound;
	style.ChildRounding = ChildRounding;
	style.PopupRounding = PopupRounding;
	style.ScrollbarRounding = ScrollbarRounding;
	style.GrabRounding = GrabRounding;
	style.TabRounding = TabRounding;

	style.WindowTitleAlign = WindowTitleAlign;
	style.ButtonTextAlign = ButtonTextAlign;
	style.SelectableTextAlign = SelectabletextAlign;

    StyleColorsMaik(&style);
}

void imgui::Cartographer::PushStyle()
{
	// initialize default window style for each window
	auto& ImDefaultStyle = ImGui::GetStyle();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, wPadding);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, fPadding);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, iSpacing);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, iiSpacing);
	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, indentSpacing);

	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, ScrollBarSize);
	ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, GrabMinSize);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, WindowBorderSize);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, ChildBorderSize);
	ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, PopupBorderSize);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, FrameBorderSize);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, WindowRound);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ChildRounding);
	ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, PopupRounding);
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, ScrollbarRounding);
	ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, GrabRounding);
	ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, TabRounding);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, WindowTitleAlign);
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ButtonTextAlign);
	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, SelectabletextAlign);
}

void imgui::Cartographer::PopStyle()
{
	ImGui::PopStyleVar(20);
}
