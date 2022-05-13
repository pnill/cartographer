#include "stdafx.h"

#include "CommandHandler.h"
#include "CommandCollection.h"

const char command_error_not_enough_params[] = "# %s command error: invalid parameter count";

ConsoleCommand::ConsoleCommand(const char* _name, const char* _command_description, int _parameter_count, ExecuteCommandCallbackT* _input_callback,
	CommandFlags _flags)
{
	m_name = _name;
	m_command_description = _command_description;
	m_flags = _flags;
	m_parameter_count = _parameter_count;
	p_exec_command_cb = _input_callback;
	m_user_data = nullptr;
}

bool ConsoleCommand::CheckArgs(ConsoleCommandCtxData* cb_data, const char* command_line, const std::vector<std::string>& tokens)
{
	IOutput* output = cb_data->strOutput;
	const ConsoleCommand* command_data = cb_data->consoleCommandData;
	if (command_data == nullptr)
		return false;

	// + 1 to count for the actual command
	if (tokens.size() != command_data->GetParameterCount() + 1)
	{
		output->OutputFmt(StringFlag_None, command_error_not_enough_params, command_data->GetName());
		return false;
	}

	return true;
}

bool ConsoleCommand::TempNameExec(const char* command_line, size_t command_line_length, const std::vector<std::string>& tokens, IOutput* ctx, ConsoleCommand* command)
{
	ConsoleCommandCtxData command_data;
	command_data.strOutput = ctx;
	command_data.commandVar = nullptr;
	command_data.consoleCommandData = command;
	IOutput* output = command_data.strOutput;

	if (CheckArgs(&command_data, command_line, tokens))
	{
		output->OutputFmt(StringFlag_History, command_line);

		if (const auto* varCommand = dynamic_cast<const ConsoleVarCommand*>(command))
		{
			command_data.commandVar = varCommand->m_var_ptr;
			command->p_exec_command_cb(tokens, command_data);
		}
		else
		{
			command->p_exec_command_cb(tokens, command_data);
		}

		return true;
	}

	return false;
}

// static function, executes command
bool ConsoleCommand::HandleCommandLine(const char* command_line, size_t command_line_length, IOutput* context)
{
    bool ret = false;

	std::vector<std::string> command_first_tokens;
	if (tokenize(command_line, command_line_length, " ", command_first_tokens))
	{
		ConsoleCommand* command = NULL;
		for (auto& command_entry : CommandCollection::commandTable)
		{
			if (_strnicmp(command_entry->GetName(), command_first_tokens[0].c_str(), strlen(command_entry->GetName())) == 0)
			{
				command = command_entry;
			}
		}

		ret = ConsoleCommand::TempNameExec(command_line, command_line_length, command_first_tokens, context, command);
	}

    return ret;
}

ConsoleVarCommand::ConsoleVarCommand(const char* _name, const char* _var_description, int _parameter_count, ExecuteCommandCallbackT* _callback, ComVar* _var_ptr, CommandFlags _flags)
    : ConsoleCommand(_name, _var_description, _parameter_count, _callback, _flags | CommandFlag_SetsVariable)
{
    m_var_ptr = _var_ptr;
	memset(m_var_str, 0, sizeof(m_var_str));
}