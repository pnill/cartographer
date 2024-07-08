#include "stdafx.h"

#include "CommandHandler.h"
#include "CommandCollection.h"

const char command_error_invalid_parameter_count[] = "# %s command error: invalid parameter count";

ConsoleCommand::ConsoleCommand(const char* _name, const char* _command_description, int _min_parameter_count, int _max_parameter_count, ExecuteCommandCallbackT* _input_callback,
	CommandFlags _flags)
{
	strncpy(m_name, _name, ARRAYSIZE(m_name));
	strncpy(m_command_description, _command_description, ARRAYSIZE(m_command_description));
	m_flags = _flags;
	m_min_parameter_count = _min_parameter_count;
	m_max_parameter_count = _max_parameter_count;
	p_exec_command_cb = _input_callback;
	SetCommandVarPtr(nullptr);
}

ConsoleCommand::ConsoleCommand(ComVarBase* _command_var, const char* _name, const char* _command_description, int _min_parameter_count, int _max_parameter_count, ExecuteCommandCallbackT* _input_callback,
	CommandFlags _flags) :
	ConsoleCommand(_name, _command_description, _min_parameter_count, _max_parameter_count, _input_callback, _flags)
{
	SetCommandVarPtr(_command_var);
}

bool ConsoleCommand::ValidateArgs(ConsoleCommandCtxData* ctxData, const char* commandLine, const std::vector<std::string>& tokens)
{
	TextOutputCb* outputCb = ctxData->outputCb;
	const ConsoleCommand* cmdData = ctxData->consoleCommand;
	if (cmdData == nullptr)
		return false;

	// + 1 to count for the actual command
	bool parameterCountInvalid = tokens.size() < cmdData->GetMinParameterCount() + 1
		|| tokens.size() > cmdData->GetMaxParameterCount() + 1;

	if (parameterCountInvalid)
	{
		if (cmdData->Hidden())
		{
			outputCb(StringFlag_None, "# unknown command: ");
			outputCb(StringFlag_History, commandLine);
		}
		else
		{
			outputCb(StringFlag_None, command_error_invalid_parameter_count, cmdData->GetName());
		}

		return false;
	}

	return true;
}

bool ConsoleCommand::ExecCommand(const char* commandLine, size_t cmdLineLength, const std::vector<std::string>& tokens, TextOutputCb* consoleOutputCb, ConsoleCommand* command)
{
	ConsoleCommandCtxData ctx;
	ctx.outputCb = consoleOutputCb;
	ctx.consoleCommand = command;

	if (ValidateArgs(&ctx, commandLine, tokens))
	{
		command->p_exec_command_cb(tokens, ctx);

		return true;
	}

	return false;
}

// static function, used to execute commands
bool ConsoleCommand::HandleCommandLine(const char* commandLine, size_t commandLineLength, TextOutputCb* consoleOutputCb)
{
    bool result = false;

	const char* delimiters = " ";
	std::vector<std::string> commandTokens;

	if (tokenize(commandLine, commandLineLength, delimiters, commandTokens))
	{
		ConsoleCommand* command = nullptr;
		for (auto cmdIt : CommandCollection::commandTable)
		{
			if (_strnicmp(cmdIt->GetName(), commandTokens[0].c_str(), strlen(cmdIt->GetName()) + 1) != 0)
				continue;
			command = cmdIt;
			break;
		}

		if (command != nullptr)
		{
			consoleOutputCb(StringFlag_History, commandLine);
			result = ExecCommand(commandLine, commandLineLength, commandTokens, consoleOutputCb, command);
		}
		else
		{
			consoleOutputCb(StringFlag_None, "# unknown command: ");
			consoleOutputCb(StringFlag_History, commandLine);
		}
	}

    return result;
}