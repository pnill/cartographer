#pragma once
#include "stdafx.h"

static std::string empty = std::string();

/// Command Variable

// useful macros
#define ComVarFromPtr(_var_name, _var_type, _var_ptr, _command_name, _description, _min_parameter_count, _max_parameter_count, _callback) \
ComVar<_var_type> _var_name##__LINE__(_var_ptr); \
ConsoleCommand _var_name((ComVarBase*)&_var_name##__LINE__, _command_name, _description, _min_parameter_count, _max_parameter_count, _callback);

class ComVarBase
{
public:
	ComVarBase() = default;
	virtual ~ComVarBase() = default;

	virtual std::string GetValStr()
	{
		return "<unimplemented>";
	}
};

template<typename T>
class CStrToValue
{
public:
	template<typename Type = T>
	static Type ToIntegral(const std::string& str, int _Base);

	template<>
	static int ToIntegral<int>(const std::string& str, int _Base)
	{
		return std::stoi(str, nullptr, _Base);
	}

	template<>
	static unsigned int ToIntegral<unsigned int>(const std::string& str, int _Base)
	{
		return std::stoul(str, nullptr, _Base);
	}

	template<>
	static long ToIntegral<long>(const std::string& str, int _Base)
	{
		return std::stol(str, nullptr, _Base);
	}

	template<>
	static long long ToIntegral<long long>(const std::string& str, int _Base)
	{
		return std::stoll(str, nullptr, _Base);
	}

	template<typename Type = T>
	static bool ToBoolean(const std::string& str)
	{
		bool result = false;
		if (str != "true"
			&& str != "false"
			&& str != "1"
			&& str != "0"
			)
			throw std::runtime_error("error: invalid boolean format (must be true/false)");
		else
			result = str == "true" ? true : false;

		return result;
	}

	template<typename Type = T>
	static Type ToFloat(const std::string& str);

	template<>
	static float ToFloat<float>(const std::string& str)
	{
		return std::stof(str);
	}

	template<>
	static double ToFloat<double>(const std::string& str)
	{
		return std::stod(str);
	}
};

template<typename T>
class ComVar : private CStrToValue<T>, public ComVarBase
{
	T* m_var_ptr;
public:
	// for custom types, the class should implement the equal operator overload
	// othewise default is used
	ComVar(T* ptr)
	{
		m_var_ptr = ptr;
	}

	virtual ~ComVar() = default;

	template<typename Type = T>
	std::enable_if_t<!std::is_same_v<Type, bool> && std::is_integral_v<Type>, bool> 
		SetFromStr(const std::string& str, int _Base = 0, std::string& potentialException = empty)
	{
		bool success = true;
		try
		{
			*m_var_ptr = this->ToIntegral<Type>(str, _Base);
		}
		catch (const std::exception& e)
		{
			success = false;
			potentialException.assign(e.what());
		}

		return success;
	}

	template<typename Type = T>
	std::enable_if_t<std::is_same_v<Type, bool>, bool>
		SetFromStr(const std::string& str, std::string& potentialException = empty)
	{
		bool success = true;
		try
		{
			*m_var_ptr = this->ToBoolean<Type>(str);
		}
		catch (const std::exception& e)
		{
			success = false;
			potentialException.assign(e.what());
		}

		return success;
	}

	template<typename Type = T>
	std::enable_if_t<std::is_floating_point_v<Type>, bool>
		SetFromStr(const std::string& str, std::string& potentialException = empty)
	{
		bool success = true;
		try
		{
			*m_var_ptr = this->ToFloat<Type>(str);
		}
		catch (const std::exception& e)
		{
			success = false;
			potentialException.assign(e.what());
		}

		return success;
	}

	template<typename Type = T>
	std::string AsString(Type* var) const
	{
		return std::to_string(*var);
	}

	template<>
	std::string AsString<bool>(bool* var) const
	{
		return *var ? "true" : "false";
	}

	std::string GetValStr() override
	{
		return AsString(m_var_ptr);
	}

	T GetVal() const
	{
		return *m_var_ptr;
	}

	void SetVal(T val)
	{
		*m_var_ptr = val;
	}
};