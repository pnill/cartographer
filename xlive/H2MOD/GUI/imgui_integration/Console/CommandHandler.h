#pragma once

#include "CommandsUtil.h"
#include "ComVar.h"

class ConsoleCommand;
struct ConsoleCommandCtxData;
typedef int CommandFlags;

// change name, this can be used to execute the command without the console
typedef int(__cdecl ExecuteCommandCallbackT)(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);

typedef int(__cdecl TextOutputCb)(StringHeaderFlags strFlags, const char* fmt, ...);

struct ConsoleCommandCtxData
{
    TextOutputCb* outputCb;
    const ConsoleCommand* consoleCommand;
};

enum CommandFlags_
{
    CommandFlag_None = 0,
    CommandFlag_Hidden = 1 << 0, // will not display in help commands or anywhere else where this flag is tested
    CommandFlag_SetsVariable = 1 << 1  // internal, do not set by yourself, but you can test it
};

class ConsoleCommand final
{
public:
    ConsoleCommand(const char* _name, const char* _command_description, int _min_parameter_count, int _max_parameter_count, ExecuteCommandCallbackT* _callback,
        CommandFlags _flags = CommandFlag_None);

    ConsoleCommand(ComVarBase* _command_var, const char* _name, const char* _command_description, int _min_parameter_count, int _max_parameter_count, ExecuteCommandCallbackT* _input_callback, CommandFlags _flags = CommandFlag_None);

    ~ConsoleCommand() = default;

    bool Hidden() const { return (m_flags & CommandFlag_Hidden) != 0;  }
    bool SetsVariable() const { return (m_flags & CommandFlag_SetsVariable) != 0;  }

    const char* GetName() const
    {
        return m_name;
    }

    const char* GetDescription() const
    {
        return m_command_description;
    }

    size_t GetMinParameterCount() const
    {
        return m_min_parameter_count;
    }

    size_t GetMaxParameterCount() const
	{
		return m_max_parameter_count;
	}

    void SetCommandVarPtr(ComVarBase* varPtr)
    {
        if (varPtr != nullptr)
        {
            m_flags |= CommandFlag_SetsVariable;
        }
        else
        {
            m_flags &= ~CommandFlag_SetsVariable;
        }

        m_var_ptr = varPtr;
    }

    template<typename T>
    T* GetVar() const
    {
        return reinterpret_cast<T*>(m_var_ptr);
    }

    CommandFlags GetFlags() const
    {
        return m_flags;
    }

    static bool ValidateArgs(ConsoleCommandCtxData* cb_data, const char* command_line, const std::vector<std::string>& tokens);
	
    static bool ExecCommand(const char* commandLine, size_t commandLineLength, const std::vector<std::string>& tokens, TextOutputCb* consoleOutputCb, ConsoleCommand* command);
   
    // handles command line
    // returns true if command line has been handled
    static bool HandleCommandLine(const char* command_line, size_t command_line_length, TextOutputCb* consoleOutputCb);

    void VarAsStr(char* outVar, size_t outSize) const
    {
        if (SetsVariable())
        {
            strncpy(outVar, m_var_ptr->GetValStr().c_str(), outSize - 1);
        }
    }

protected:

    CommandFlags m_flags;
    char m_name[32];
    char m_command_description[256];
    size_t m_min_parameter_count;
    size_t m_max_parameter_count;
    ComVarBase* m_var_ptr;

private:
    ExecuteCommandCallbackT* p_exec_command_cb;
};
