#pragma once

#include "imgui.h"
#include "..\ImGui_Handler.h"
#include "ComVar.h"
#include "CommandHandler.h"
#include "CommandCollection.h"
#include "H2MOD\Modules\Input\PlayerControl.h"

class Console;

Console* GetMainConsoleInstance();
void ImGui_Console_OpenDefault(const char*, bool*);

class Console : IOutput
{
private:
    // variables
    bool                                m_auto_scroll;
    bool                                m_scroll_to_botom;
    bool                                m_reclaim_input_box_focus = false;

    char                                m_input_buffer[MAX_CONSOLE_INPUT_BUFFER];
    CircularStringBuffer                m_output;
    ImGuiTextInputCompletion*           m_completion_data;
    CircularStringBuffer                m_completion_text_buffer;
    int                                 m_history_string_index;
    ImGuiTextFilter                     m_filter;

    static int TextEditCallback(ImGuiInputTextCallbackData* data);

    void ExecCommand(const char* command_line, size_t command_line_length);

    void ClearOutput();

public:
    ComVarT<float>                     m_console_opacity;

    Console();
    ~Console() = default;

    Console(const Console&) = delete;
    Console(Console&&) = delete;
    Console operator=(Console& other) = delete;

    int Output(StringHeaderFlags flags, const char* fmt) override;
    int OutputFmt(StringHeaderFlags flags, const char* fmt, ...) override;

    void Draw(const char* title, bool* p_open);

    void AllocateCompletionCandidatesBuf(unsigned int candidates_count);
    void DiscardCompletionCandidatesBuf();

    bool CompletionAvailable() const
    {
        return m_completion_data != NULL ? true : false;
    };

    unsigned int GetCompletionCandidatesCount() const {
        if (!CompletionAvailable()) return 0;
        return m_completion_data->Count;
    };

    static int help_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
    static int clear_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
    static int d3d9ex_set_state_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
    static int set_opacity_cb(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
 
    static void Open()
    {
        GetMainConsoleInstance()->m_reclaim_input_box_focus = true;
    }
    static void Close()
    {
    }

    static void Render(bool* b_open)
    {
        ImGui_Console_OpenDefault("console", b_open);
    }
};

