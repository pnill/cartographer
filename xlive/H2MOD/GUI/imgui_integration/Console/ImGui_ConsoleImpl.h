#pragma once

#include "imgui.h"
#include "..\ImGui_Handler.h"
#include "ComVar.h"
#include "CommandHandler.h"
#include "CommandCollection.h"
#include "H2MOD\Modules\Input\PlayerControl.h"

class Console;

// used to get the main console instance
Console* GetMainConsoleInstance();

#define CONSOLE_TABS 2

static const char* console_tab_name[CONSOLE_TABS] = {
    "Console commands",
    "Logs"
};

enum ConsoleTabs
{
    _console_tab_commands,
	_console_tab_logs,

    _console_tab_end
};

class Console : public IOutput
{
private:
	Console(const Console&) = delete;
	Console(Console&&) = delete;
	Console operator=(Console&) = delete;

    // variables
    bool                                m_auto_scroll;
    bool                                m_scroll_to_botom;
    bool                                m_reclaim_input_box_focus = false;

    char                                m_input_buffer[MAX_CONSOLE_INPUT_BUFFER];
    ImGuiTextInputCompletion*           m_completion_data;
    CircularStringBuffer                m_completion_text_buffer;
    int                                 m_history_string_index;
    ImGuiTextFilter                     m_filter;
    int                                 m_selected_tab;
    bool                                m_selected_tab_dirty;
	std::vector<CircularStringBuffer>
                                        m_output;

    static int TextEditCallback(ImGuiInputTextCallbackData* data);

    void ExecCommand(const char* command_line, size_t command_line_length);

    void ClearMainOutput();

    CircularStringBuffer* GetMainOutput()
    {
        return &m_output.data()[m_selected_tab];
    };

public:
    static std::string                 windowName;
    ComVarT<float>                     m_console_opacity;

    Console();
    ~Console() = default;

    int Output(StringHeaderFlags flags, const char* fmt) override;
    int OutputFmt(StringHeaderFlags flags, const char* fmt, ...) override;

	void Draw(const char* title, bool* p_open);

    void AllocateCompletionCandidatesBuf(unsigned int candidates_count);
    void DiscardCompletionCandidatesBuf();

    void SelectTab(ConsoleTabs tab);

    bool CompletionAvailable() const
    {
        return m_completion_data != NULL;
    };

    unsigned int GetCompletionCandidatesCount() const {
        if (!CompletionAvailable()) return 0;
        return m_completion_data->Count;
    };

	CircularStringBuffer* GetTabOutput(ConsoleTabs tab)
	{
		return &m_output.data()[tab];
	};
 
    static void Open()
    {
        GetMainConsoleInstance()->m_reclaim_input_box_focus = true;
    }
    static void Close()
    {
    }
    static void Render(bool* b_open)
    {
		auto console = GetMainConsoleInstance();
		console->Draw(windowName.c_str(), b_open);
    }

    // commands
	static int clear_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int set_opacity_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
};

