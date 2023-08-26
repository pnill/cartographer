#pragma once

#include "CommandsUtil.h"

#include "ComVar.h"

class ConsoleLog;
class ConsoleCommand;
struct ConsoleCommandCtxData;
typedef int CommandFlags;

// change name, this can be used to execute the command without the console
typedef int(ExecuteCommandCallbackT)(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);

// for redirecting output to either dedicated server/imgui console
class ConsoleLog
{
public:
	virtual int Output(StringHeaderFlags flags, const char* fmt, ...) = 0;
};

struct ConsoleCommandCtxData
{
    ConsoleLog* strOutput;
    IComVar* commandVar;
    ExecuteCommandCallbackT* execCmdCb;
    const ConsoleCommand* consoleCommandData;
};

enum CommandFlags_
{
    CommandFlag_None = 0,
    CommandFlag_Hidden = 1 << 0, // will not display in help commands or anywhere else where this flag is tested
    CommandFlag_SetsVariable = 1 << 1  // internal, do not set by yourself, but you can test it
};

class ConsoleCommand
{
public:
    bool Hidden() const { return (m_flags & CommandFlag_Hidden) != 0;  }
    bool CommandSetsVariable() const { return (m_flags & CommandFlag_SetsVariable) != 0;  }

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

    template<typename T = void*> T GetUserData() const 
    { 
        return (T)m_user_data; 
    }

    CommandFlags GetFlags() const
    {
        return m_flags;
    }

    static bool CheckArgs(ConsoleCommandCtxData* cb_data, const char* command_line, const std::vector<std::string>& tokens);
	
    static bool ExecCommand(const char* command_line, size_t command_line_length, const std::vector<std::string>& tokens, ConsoleLog* output, ConsoleCommand* command);
   
    ConsoleCommand(const char* _name, const char* _command_description, int _min_parameter_count, int _max_parameter_count, ExecuteCommandCallbackT* _callback,
        CommandFlags _flags = CommandFlag_None);

    virtual ~ConsoleCommand() = default;

    // handles command line
    // returns true if command line has been handled
    static bool HandleCommandLine(const char* command_line, size_t command_line_length, ConsoleLog* context);

protected:

    CommandFlags m_flags;
    const char* m_name;
    const char* m_command_description;
    size_t m_min_parameter_count;
    size_t m_max_parameter_count;
    void* m_user_data;

private:
    ExecuteCommandCallbackT* p_exec_command_cb;
};

// this is needed because we want to display the value inside the 
// and display it in 
class ConsoleVarCommand final : public ConsoleCommand
{
public:
    ConsoleVarCommand(const char* _name, const char* _var_description, int _min_parameter_count, int _max_parameter_count, ExecuteCommandCallbackT* _callback, IComVar* _var_ptr = nullptr, CommandFlags _flags = CommandFlag_None);
    
    ~ConsoleVarCommand() override = default;

    void UpdateVarPtr(IComVar* varPtr)
    {
        m_var_ptr = varPtr;
    }

    const char* VarAsStr()
    {
        strncpy(m_var_str, m_var_ptr->GetValStr().c_str(), sizeof(m_var_str));
        return m_var_str;
    }

    IComVar* m_var_ptr;
    char m_var_str[1024];

private:
};


