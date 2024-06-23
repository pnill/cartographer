#include "stdafx.h"

#include "CommandHandler.h"
#include "CommandCollection.h"

const char command_error_not_enough_params[] = "# %s command error: invalid parameter count";

ConsoleCommand::ConsoleCommand(const char* _name, const char* _command_description, int _min_parameter_count, int _max_parameter_count, ExecuteCommandCallbackT* _input_callback,
	CommandFlags _flags)
{
	strncpy(m_name, _name, ARRAYSIZE(m_name) - 1);
	strncpy(m_command_description, _command_description, ARRAYSIZE(m_command_description) - 1);
	m_flags = _flags;
	m_min_parameter_count = _min_parameter_count;
	m_max_parameter_count = _max_parameter_count;
	p_exec_command_cb = _input_callback;
	SetCommandVarPtr(nullptr);
}

ConsoleCommand::ConsoleCommand(IComVar* _command_var, const char* _name, const char* _command_description, int _min_parameter_count, int _max_parameter_count, ExecuteCommandCallbackT* _input_callback,
	CommandFlags _flags) :
	ConsoleCommand(_name, _command_description, _min_parameter_count, _max_parameter_count, _input_callback, _flags)
{
	SetCommandVarPtr(_command_var);
}

bool ConsoleCommand::CheckArgs(ConsoleCommandCtxData* cb_data, const char* command_line, const std::vector<std::string>& tokens)
{
	ConsoleLog* output = cb_data->strOutput;
	const ConsoleCommand* command_data = cb_data->consoleCommandData;
	if (command_data == nullptr)
		return false;

	// + 1 to count for the actual command
	if (tokens.size() < command_data->GetMinParameterCount() + 1
		|| tokens.size() > command_data->GetMaxParameterCount() + 1)
	{
		if (command_data->Hidden())
		{
			output->Output(StringFlag_None, "# unknown command: ");
			output->Output(StringFlag_History, command_line);
		}
		else
		{
			output->Output(StringFlag_None, command_error_not_enough_params, command_data->GetName());
		}

		return false;
	}

	return true;
}

bool ConsoleCommand::ExecCommand(const char* command_line, size_t command_line_length, const std::vector<std::string>& tokens, ConsoleLog* output, ConsoleCommand* command)
{
	ConsoleCommandCtxData command_data;
	command_data.strOutput = output;
	command_data.commandVar = nullptr;
	command_data.consoleCommandData = command;

	if (CheckArgs(&command_data, command_line, tokens))
	{
		if (command->CommandSetsVariable())
		{
			command_data.commandVar = command->m_var_ptr;
		}

		command->p_exec_command_cb(tokens, command_data);

		return true;
	}

	return false;
}

// static function, used to execute commands
bool ConsoleCommand::HandleCommandLine(const char* command_line, size_t command_line_length, ConsoleLog* output)
{
    bool ret = false;

	std::vector<std::string> command_first_tokens;
	const char* delimiters = " ";
	if (tokenize(command_line, command_line_length, delimiters, command_first_tokens))
	{
		ConsoleCommand* command = nullptr;
		for (auto command_entry : CommandCollection::commandTable)
		{
			if (_strnicmp(command_entry->GetName(), command_first_tokens[0].c_str(), strlen(command_entry->GetName()) + 1) != 0)
				continue;
			command = command_entry;
			break;
		}

		if (command != nullptr)
		{
			output->Output(StringFlag_History, command_line);
			ret = ConsoleCommand::ExecCommand(command_line, command_line_length, command_first_tokens, output, command);
		}
		else
		{
			output->Output(StringFlag_None, "# unknown command: ");
			output->Output(StringFlag_History, command_line);
		}
	}

    return ret;
}