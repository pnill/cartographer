#pragma once

#include "imgui.h"
#include "ComVar.h"
#include "CommandHandler.h"

class CartographerConsole;
typedef CartographerConsole CartConsole;

// used to get the main console instance
CartographerConsole* GetMainConsoleInstance();

#define CONSOLE_TABS 2

static const char* console_tab_name[CONSOLE_TABS] = 
{
    "Console commands",
    "Logs"
};

enum ConsoleTabs
{
    _console_tab_commands,
	_console_tab_logs,

    _console_tab_end
};

class CartographerConsole
{
private:
	CartographerConsole(const CartographerConsole&) = delete;
	CartographerConsole(CartographerConsole&&) = delete;
	CartographerConsole operator=(CartographerConsole&) = delete;

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

    bool                                m_docked;

    static int TextEditCallback(ImGuiInputTextCallbackData* data);

    void ExecCommand(const char* command_line, size_t command_line_length);

    static void ClearMainOutput();

    CircularStringBuffer* GetMainOutput()
    {
        return &m_output.data()[m_selected_tab];
    };

public:
    static std::string                 windowName;
    float                              m_console_opacity;
    ComVar<float>                      m_console_opacity_comvar;

    CartographerConsole();
    ~CartographerConsole() = default;

	void Draw(const char* title, bool* p_open);

    void AllocateCompletionCandidatesBuf(unsigned int candidates_count);
    void DiscardCompletionCandidatesBuf();

    void SwitchToTab(ConsoleTabs tab);

    bool CompletionAvailable() const
    {
        return m_completion_data != NULL;
    };

    unsigned int GetCompletionCandidatesCount() const 
    {
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

    static void LogToTab(ConsoleTabs tab, const char* fmt, ...)
    {
		va_list valist;
		va_start(valist, fmt);
        GetMainConsoleInstance()->GetTabOutput(tab)->AddStringFmt(StringFlag_None, fmt, valist);
        va_end(valist);
    }

    static int __cdecl LogToMainTabCb(StringHeaderFlags flags, const char* fmt, ...);

    // commands
	static int clear_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int set_opacity_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
};


#define QUICK_DBG(header, fmt, ...) \
do { \
CartographerConsole::LogToTab(_console_tab_logs, \
	header ": " "   %s() -> " fmt, __FUNCTION__, __VA_ARGS__); \
} while (0)

#define SIM_EVENT_QUEUE_DBG(fmt, ...)
//#define SIM_EVENT_QUEUE_DBG(fmt, ...) QUICK_DBG("sim-event-queue", fmt, __VA_ARGS__)

#define SIM_ENT_QUEUE_DBG(fmt, ...)
//#define SIM_ENT_QUEUE_DBG(fmt, ...) QUICK_DBG("sim-ent-queue", fmt, __VA_ARGS__)