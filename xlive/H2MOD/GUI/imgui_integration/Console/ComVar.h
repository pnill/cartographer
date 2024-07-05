#pragma once
#include "stdafx.h"

static std::string empty = std::string();

// remove pointer

/// Command Variable

// useful macros
#define ComVarFromPtr(_var_name, _var_type, _var_ptr, _command_name, _description, _min_parameter_count, _max_parameter_count, _callback) \
ComVarT<_var_type> _var_name##__LINE__(_var_ptr); \
ConsoleCommand _var_name((IComVar*)&_var_name##__LINE__, _command_name, _description, _min_parameter_count, _max_parameter_count, _callback);

class IComVar
{
public:
	IComVar() = default;
	virtual ~IComVar() = default;

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

template<typename T, typename baseTypeT = typename std::remove_all_pointers<T>::type>
class ComVarT : private CStrToValue<baseTypeT>, public IComVar
{
	static_assert(!std::is_same_v<baseTypeT, T>
		// not caring about the const versions, they shouldn't be const in the first place
		// || !std::is_same<const std::remove_all_pointers<T>, T>::value
		// || !std::is_same<std::remove_all_pointers<T> const, T>::value
		, "ComVarT: template parameter not a pointer");

	baseTypeT* m_var_ptr;
public:
	// for custom types, the class should implement the equal operator overload
	// othewise default is used
	ComVarT(baseTypeT* ptr)
	{
		m_var_ptr = ptr;
	}

	virtual ~ComVarT() = default;

	template<typename Type = baseTypeT>
	std::enable_if_t<!std::is_same_v<Type, bool>&& std::is_integral_v<Type>, Type> 
		SetValFromStr(const std::string& str, int _Base = 0, std::string& potentialException = empty)
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

	template<typename Type = baseTypeT>
	std::enable_if_t<std::is_same_v<Type, bool>, bool>
		SetValFromStr(const std::string& str, std::string& potentialException = empty)
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

	template<typename Type = baseTypeT>
	std::enable_if_t<std::is_floating_point_v<Type>, bool>
		SetValFromStr(const std::string& str, std::string& potentialException = empty)
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

	template<typename Type = baseTypeT>
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

	baseTypeT GetVal() const
	{
		return *m_var_ptr;
	}

	void SetVal(baseTypeT val)
	{
		*m_var_ptr = val;
	}
};

template<typename T>
class ComVar : public ComVarT<T*>
{
	static_assert(!std::is_pointer_v<T>
		|| std::is_same_v<std::remove_all_pointers<T>::type, T>
		, "ComVar: template parameter is invalid (possibly a pointer instead of a type)");

	T m_var;

public:
	ComVar() :
		ComVarT<T*>(&m_var)
	{
	}

	ComVar(T value) :
		ComVarT<T*>(&m_var)
	{
		m_var = value;
	}

	virtual ~ComVar() = default;
};