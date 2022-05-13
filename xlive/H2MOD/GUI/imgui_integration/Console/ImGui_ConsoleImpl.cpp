
#include "stdafx.h"
#include "ImGui_ConsoleImpl.h"

#include "H2MOD\Modules\Shell\Config.h"

#define CONSOLE_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

const char command_error_bad_arg[] = "# exception catch (bad arg): ";

ConsoleVarCommand console_opacity_var_cmd("var_console_opacity", "set console opacity, 1 parameter(s): <float>", 1, Console::set_opacity_cb);

// DECL_ComVarCommandPtr(d3d9ex_var, bool*, &H2Config_d3dex, "var", "enable/disable d3d9ex, 1 parameter(s): <bool>", 1, Console::d3d9x_set_state);

Console* GetMainConsoleInstance()
{
	static std::unique_ptr<Console> console(std::make_unique<Console>());
	return console.get();
}

Console::Console() :
	m_output(256, MAX_CONSOLE_INPUT_BUFFER),
	m_completion_text_buffer(32, 64),
	m_console_opacity(.75f)
{
	m_auto_scroll = true;
	m_scroll_to_botom = false;
	m_history_string_index = -1;
	m_completion_data = NULL;
	memset(m_input_buffer, 0, sizeof(m_input_buffer));

	// you can pass nullptr to ImGui_ConsoleVar if you can get the variable from context data
	console_opacity_var_cmd.UpdateVarPtr((ComVar*)&m_console_opacity);
	CommandCollection::InsertCommand(&console_opacity_var_cmd);
	CommandCollection::InsertCommand(new ConsoleCommand("help", "outputs all commands, 0 parameter(s)", 0, Console::help_cb));
	CommandCollection::InsertCommand(new ConsoleCommand("clear", "clear the output of the current console and history, 0 parameter(s)", 0, Console::clear_cb));
}

int Console::Output(StringHeaderFlags flags, const char* fmt)
{
	m_output.AddString(flags, fmt);
	return 0;
}

int Console::OutputFmt(StringHeaderFlags flags, const char* fmt, ...)
{
	va_list valist;
	va_start(valist, fmt);
	m_output.AddStringFmt(flags, fmt, valist);
	va_end(valist);
	return 0;
}

// ImGui text callback
int Console::TextEditCallback(ImGuiInputTextCallbackData* data)
{
	Console* console_data = (Console*)data->UserData;
	data->CompletionData = NULL;
	bool skip_completion_find = false;

	// first update the state of completion data
	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackEdit:
	case ImGuiInputTextFlags_CallbackAlways:
	case ImGuiInputTextFlags_CallbackHistory:
	case ImGuiInputTextFlags_CallbackCompletion:
		if (data->BufTextLen > 0
			&& (data->Buf[data->BufTextLen - 1] == ' '
				//|| data->Buf[data->BufTextLen - 1] == '\n'
				// || data->Buf[data->BufTextLen - 1] == '\t'
				)
			|| data->BufTextLen == 0
			)
		{
			skip_completion_find = true;
			break;
		}

		if (data->BufTextLen > 0
			&& console_data->CompletionAvailable()
			&& console_data->GetCompletionCandidatesCount() > 0
			&& console_data->m_history_string_index == -1)
		{
			data->CompletionData = console_data->m_completion_data;
		}
		break;
	}

	// then handle the event
	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackEdit:
	{
		// don't allow spaces when no other character have been inserted
		if (data->BufTextLen == 1 && data->Buf[data->BufTextLen - 1] == ' ')
		{
			// delete the space character
			data->DeleteChars(data->BufTextLen - 1, 1);
			break;
		}

		// reset the history index if we modified the buffer
		console_data->m_history_string_index = -1;

		if (skip_completion_find)
			break;

		std::vector<ConsoleCommand*> completion_commands;
		for (auto command_ptr : CommandCollection::commandTable)
		{
			if (InputTextContainsCommandSubstring(command_ptr->GetName(), data->Buf, true))
			{
				completion_commands.push_back(command_ptr);
			}
		}

		if (completion_commands.size() > 1)
		{
			std::sort(completion_commands.begin(), completion_commands.end(), [](const ConsoleCommand* a, const ConsoleCommand* b) -> bool
				{
					return strcmp(a->GetName(), b->GetName()) < 0;
				}
			);
		}

		if (console_data->m_completion_data == NULL
			|| (console_data->m_completion_data != NULL
				&& completion_commands.size() > console_data->GetCompletionCandidatesCount()))
		{
			// this also clears the old buffer
			console_data->AllocateCompletionCandidatesBuf(completion_commands.size());
		}

		// in case we didn't need to re-allocate memory, set the count and selected index
		console_data->m_completion_data->SelectedCandidateIndex = -1;
		console_data->m_completion_data->Count = completion_commands.size();

		for (int i = 0; i < completion_commands.size(); i++)
		{
			ConsoleCommand* command = completion_commands[i];
			memset(&console_data->m_completion_data->CompletionCandidate[i], 0, sizeof(ImGuiTextInputCompletionCandidate));
			console_data->m_completion_data->CompletionCandidate[i].CompletionText = command->GetName();
			console_data->m_completion_data->CompletionCandidate[i].CompletionVariable = nullptr;
			console_data->m_completion_data->CompletionCandidate[i].CompletionDescription = command->GetDescription();

			if (auto * var_command = dynamic_cast<ConsoleVarCommand*>(command))
			{
				console_data->m_completion_data->CompletionCandidate[i].CompletionVariable = var_command->VarAsStr();
			}
		}

		// after we found the candidates, update the callback data with the pointers to the buffer
		data->CompletionData = console_data->m_completion_data;
	}
	break;

	case ImGuiInputTextFlags_CallbackHistory:
	{
		// if we have completion data, we do not want to use history
		// unless the current string is actually history
		if (data->BufTextLen > 0
			&& console_data->CompletionAvailable()
			&& console_data->m_history_string_index == -1)
		{
			const int prev_selected_completion_index = console_data->m_completion_data->SelectedCandidateIndex;
			if (data->EventKey == ImGuiKey_UpArrow)
				console_data->m_completion_data->SelectedCandidateIndex--;
			else if (data->EventKey == ImGuiKey_DownArrow)
				console_data->m_completion_data->SelectedCandidateIndex++;

			// clamp between -1 and string header size - 1
			console_data->m_completion_data->SelectedCandidateIndex = CONSOLE_CLAMP(console_data->m_completion_data->SelectedCandidateIndex, -1, (int)(console_data->GetCompletionCandidatesCount() - 1));
		}
		// check if we actually have something to display first
		else if (console_data->m_output.GetStringHeaderSize() > 0)
		{
			const int prev_history_index = console_data->m_history_string_index;
			if (data->EventKey == ImGuiKey_UpArrow)
				console_data->m_history_string_index++;
			else if (data->EventKey == ImGuiKey_DownArrow)
				console_data->m_history_string_index--;

			// clamp between -1 and string header size - 1
			console_data->m_history_string_index = CONSOLE_CLAMP(console_data->m_history_string_index, -1, (int)(console_data->m_output.GetStringHeaderSize() - 1));

			// TODO cleanup and maybe? simplify
			// wrote this at ~ 1 am and hackily fixed the bugs the next day
			if (prev_history_index != console_data->m_history_string_index)
			{
				if (console_data->m_history_string_index >= 0)
				{
					const char* new_text_box_from_history = NULL;
					for (int i = (int)console_data->m_output.GetStringHeaderSize() - 1; i >= 0; i--)
					{
						auto& string_header = console_data->m_output.GetHeader(i);
						int string_header_index_to_history_index = (int)console_data->m_output.GetStringHeaderSize() - 1 - i;
						if (string_header.flags & StringFlag_History)
						{
							if ((prev_history_index == -1 && console_data->m_history_string_index > prev_history_index)
								|| (console_data->m_history_string_index > prev_history_index && prev_history_index < string_header_index_to_history_index)
								|| (console_data->m_history_string_index < prev_history_index && prev_history_index > string_header_index_to_history_index)
								)
							{
								new_text_box_from_history = console_data->m_output.GetStringAtIndex(i);
								console_data->m_history_string_index = string_header_index_to_history_index;

								// if we are scrolling back down, don't stop at the first string we find
								if ((prev_history_index == -1 && console_data->m_history_string_index > prev_history_index)
									|| (console_data->m_history_string_index > prev_history_index && prev_history_index < string_header_index_to_history_index))
								{
									break;
								}
							}
							// if there's just one history string available, and we hit the button down
							// clear the text from the console
							// instead of clearing it everytime
							else if (i == 0 && prev_history_index == string_header_index_to_history_index)
							{
								data->DeleteChars(0, data->BufTextLen);
							}
						}
						// if there's no other string marked as history when using up key, just reset the index and then break
						// so we don't actually increase it for no reason
						else if (i == 0)
						{
							if (console_data->m_history_string_index == string_header_index_to_history_index)
							{
								console_data->m_history_string_index = prev_history_index;
							}
						}
					}

					// if history text is NULL, it means we didn't find any hostory inside the output buffer
					if (new_text_box_from_history != NULL)
					{
						data->DeleteChars(0, data->BufTextLen);
						data->InsertChars(0, new_text_box_from_history);
					}
					// if we hit key down, and we didn't find any history, just reset
					else if (prev_history_index > console_data->m_history_string_index) 
					{
						data->DeleteChars(0, data->BufTextLen);
					}
				}
				else
				{
					data->DeleteChars(0, data->BufTextLen);
				}
			}
		}
	}
	break;

	case ImGuiInputTextFlags_CallbackAlways:
		// every frame we pass the cached completion candidates

		// TODO FIXME for some reason clicking the completion candidate doesn't work
		/*if (data->BufTextLen > 0
			&& console_data->CompletionAvailable()
			&& console_data->GetCompletionCandidatesCount() > 0
			&& console_data->m_history_string_index == -1)
		{
			if (console_data->m_completion->ClickedCandidateIndex != -1)
			{
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, console_data->m_completion->CompletionCandidate[console_data->m_completion->ClickedCandidateIndex].CompletionText);
				console_data->m_completion->ClickedCandidateIndex = -1;
			}
		}*/

		break;

	case ImGuiInputTextFlags_CallbackCompletion:
		// here we handle TAB key press
		// we just override the buffer with the command at the current completion index
		if (data->BufTextLen > 0
			&& console_data->CompletionAvailable()
			&& console_data->m_completion_data->SelectedCandidateIndex != -1
			&& console_data->m_history_string_index == -1)
		{
			data->DeleteChars(0, data->BufTextLen);
			data->InsertChars(0, console_data->m_completion_data->CompletionCandidate[console_data->m_completion_data->SelectedCandidateIndex].CompletionText);
			console_data->m_completion_data->SelectedCandidateIndex = -1;
		}

		break;


	case ImGuiInputTextFlags_CallbackCharFilter:
	case ImGuiInputTextFlags_CallbackResize:
		break;
	}

	return 0;
}

void Console::ClearOutput() 
{
	m_output.Clear();
};

void Console::ExecCommand(const char* command_line, size_t command_line_length)
{
	m_history_string_index = -1;

	if (!ConsoleCommand::HandleCommandLine(command_line, command_line_length, this))
	{
		m_output.AddString(StringFlag_None, "# unknown command: ");
		m_output.AddString(StringFlag_History, command_line);
		m_scroll_to_botom = true;
	}
}

void Console::AllocateCompletionCandidatesBuf(unsigned int candidates_count)
{
	DiscardCompletionCandidatesBuf();
	// TODO maybe move this to function for object using dynamic allocated memory
	unsigned int buffer_size = sizeof(ImGuiTextInputCompletion) + sizeof(ImGuiTextInputCompletionCandidate) * candidates_count;
	m_completion_data = (ImGuiTextInputCompletion*)new char[buffer_size];
	memset(m_completion_data, 0, buffer_size);
	new (m_completion_data) ImGuiTextInputCompletion(candidates_count); // new here calls constructor
	// completion candidates are stored contiguously
	m_completion_data->CompletionCandidate = (ImGuiTextInputCompletionCandidate*)((char*)m_completion_data + sizeof(ImGuiTextInputCompletion));
}

void Console::DiscardCompletionCandidatesBuf()
{
	if (m_completion_data != NULL)
	{
		char* buffer_to_delete = (char*)m_completion_data; // initially just char buffer
		delete[] buffer_to_delete;
	}
	m_completion_data = NULL;
}

void Console::Draw(const char* title, bool* p_open)
{
	if (!*p_open) return;

	static bool  console_window_initialized = false;

	ImGuiIO& io = ImGui::GetIO();
	const ImGuiStyle& style = ImGui::GetStyle();
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowSizeConstraints(
		ImVec2(main_viewport->WorkSize.x, main_viewport->WorkSize.y / 3.0f), 
		ImVec2(main_viewport->WorkSize.x, main_viewport->WorkSize.y / 2.0f)
	);

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver); // in the left upside corner

	ImGuiWindowFlags console_main_window_flags = 0
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoSavedSettings
		// | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_MenuBar
		//| ImGuiWindowFlags_NoScrollbar
		//| ImGuiWindowFlags_NoScrollWithMouse
		//| ImGuiWindowFlags_AlwaysAutoResize
		;

	ImGui::SetNextWindowBgAlpha(m_console_opacity.GetVal());

	if (!ImGui::Begin(title, p_open, console_main_window_flags))
	{
		ImGui::End();
		return;
	}

	static bool bring_console_window_to_focus = false;
	bring_console_window_to_focus = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Clear")) { ClearOutput(); }
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// Poput Context Window
	if (ImGui::BeginPopupContextWindow(NULL, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
	{
		if (ImGui::MenuItem("Close Window")) { *p_open = false; }
		ImGui::EndPopup();
	}

	ImGui::Separator();

	// console text window
	ImGuiWindowFlags console_log_child_window_flags = 0
		| ImGuiWindowFlags_HorizontalScrollbar
		;

	//float footer_height_to_reserve = 0.0f;
	float footer_height_to_reserve = (ImGui::GetFrameHeightWithSpacing());
	ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_FrameBg]);
	ImGui::SetNextWindowBgAlpha(m_console_opacity.GetVal());
	ImGui::BeginChild("##consolelog", ImVec2(-1, -footer_height_to_reserve), true, console_log_child_window_flags);
	
	// Child log Poput Context Window
	if (ImGui::BeginPopupContextWindow(NULL, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
	{
		if (m_output.GetStringHeaderSize() > 0)
			if (ImGui::MenuItem("Clear")) { ClearOutput(); }
		if (ImGui::MenuItem("Close Window")) { *p_open = false; }
		ImGui::EndPopup();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
	for (int i = 0; i < m_output.GetStringHeaderSize(); i++)
	{
		// TODO add color support
		char console_text_id[512];
		snprintf(console_text_id, ARRAYSIZE(console_text_id), "##consolelogid%d", i);
		ImGui::PushID(console_text_id);

		auto& stringHeader = m_output.GetHeader(i);
		const char* stringToAdd = m_output.GetStringAtOffset(stringHeader.offset);
		if (stringHeader.flags & StringFlag_CopyToClipboard)
			ImGui::LogToClipboard();

		ImGui::Text(stringToAdd, stringToAdd + (stringHeader.size - 1));

		if (stringHeader.flags & StringFlag_CopyToClipboard)
		{
			stringHeader.flags &= ~(StringFlag_CopyToClipboard);
			ImGui::LogFinish();
		}

		if (ImGui::BeginPopupContextItem("##copy to clipboard"))
		{
			if (ImGui::MenuItem("Copy to clipboard")) { stringHeader.flags |= StringFlag_CopyToClipboard; }
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}
	ImGui::PopStyleVar();

	if (m_scroll_to_botom || (m_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		ImGui::SetScrollHereY(1.0f);
	m_scroll_to_botom = false;

	ImGui::PopStyleColor();
	ImGui::EndChild();

	static bool reclaim_input_box_focus = false;

	// ImGui::PushItemWidth(-(ImGui::CalcTextSize("Enter").x + style.FramePadding.x * 2.0f + 5.0f + 1.0f));
	ImGuiInputTextFlags input_text_flags = 0
		| ImGuiInputTextFlags_EnterReturnsTrue
		| ImGuiInputTextFlags_AutoSelectAll
		| ImGuiInputTextFlags_CallbackCompletion
		| ImGuiInputTextFlags_CallbackHistory
		| ImGuiInputTextFlags_CallbackEdit
		| ImGuiInputTextFlags_CallbackAlways
		| ImGuiInputTextFlags_DisplaySuggestions
		;

	ImGui::SetNextItemWidth(-(ImGui::CalcTextSize("Enter").x + style.FramePadding.x * 2.0f + 5.0f + 1.0f));
	if (ImGui::InputTextWithHint("##command1", "<command>", m_input_buffer, IM_ARRAYSIZE(m_input_buffer), input_text_flags, TextEditCallback, this))
	{
		size_t input_buffer_string_length = strnlen_s(m_input_buffer, sizeof(m_input_buffer) - 1);
		if (input_buffer_string_length > 0)
		{
			ExecCommand(m_input_buffer, input_buffer_string_length);
			memset(m_input_buffer, 0, 2);
			reclaim_input_box_focus = true;
		}
	}
	bool input_text_active = ImGui::IsItemActive();

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_input_box_focus)
	{
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
		reclaim_input_box_focus = false;
	}

	//ImGui::PopItemWidth();

	// enter button
	ImGui::SameLine(0.0f, 5.0f);
	if (ImGui::Button("Enter"))
	{
		size_t input_buffer_string_length = strnlen_s(m_input_buffer, sizeof(m_input_buffer) - 1);
		if (input_buffer_string_length > 0)
		{
			ExecCommand(m_input_buffer, input_buffer_string_length);
			memset(m_input_buffer, 0, 2);
		}
	}

	if (input_text_active)
	{
		// Command-line
		// ImGui::Separator();

		ImGuiWindowFlags completion_window_flags = 0
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoFocusOnAppearing
			// | ImGuiWindowFlags_NoResize
			;

		// window cursor positon 
		ImVec2 main_window_last_screen_pos_cursor = ImGui::GetCursorScreenPos(); // will use this to display the command completion candidates

		ImVec2 req_text_input_window_size
			= ImVec2(main_viewport->WorkSize.x - style.WindowPadding.x - style.ItemSpacing.x, 0.0f);
	}

	console_window_initialized = true;

	ImGui::End();
}
void ImGui_Console_OpenDefault(const char* title, bool* p_open)
{
	auto console = GetMainConsoleInstance();
	console->Draw(title, p_open);
}

// -------------
// commands
// -------------

// TODO remove function prolog code duplication

// clear command
int Console::clear_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	Console* console_data = (Console*)cbData.strOutput;
	console_data->ClearOutput();
	return 0;
}

// help command
int Console::help_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = cbData.strOutput;
	const ConsoleCommand* command_data = cbData.consoleCommandData;
	
	output->Output(StringFlag_None, "# available commands: ");

	for (auto& command_entry : CommandCollection::commandTable)
	{
		if ((command_entry->GetFlags() & CommandFlag_Hidden) == 0)
		{
			output->OutputFmt(StringFlag_None, "# %s ", command_entry->GetName());
			if (command_entry->GetDescription() != NULL)
			{
				output->OutputFmt(StringFlag_None, "    # command description: %s", command_entry->GetDescription());
			}
		}
	}
	return 0;
}

// set opacity command
int Console::set_opacity_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	Console* console_data = (Console*)cbData.strOutput;
	const ConsoleCommand* command_data = cbData.consoleCommandData;

	std::string exception;
	if (!console_data->m_console_opacity.SetValFromStr(tokens[1], 10, exception))
	{
		console_data->m_output.AddString(StringFlag_None, command_error_bad_arg);
		console_data->m_output.AddStringFmt(StringFlag_None, "	%s", exception.c_str());
	}
	return 0;
}

int Console::d3d9ex_set_state_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	Console* console_data = (Console*)cbData.strOutput;
	return 0;
}
