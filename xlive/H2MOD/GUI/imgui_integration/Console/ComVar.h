#pragma once

#include "stdafx.h"

// remove pointer

/// Command Variable

// useful macros
#define ComVarFromPtr(_var_name, _var_type, _var_ptr, _command_name, _description, _min_parameter_count, _max_parameter_count, _callback) \
ComVarT<_var_type> _var_name(_var_ptr); \
ConsoleVarCommand _var_name## _cmd(_command_name, _description, _min_parameter_count, _max_parameter_count, _callback, (IComVar*)&_var_name);

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
class ComVarT : public IComVar
{
	using baseTypeT = typename std::remove_all_pointers<T>::type;

	static_assert(!std::is_same_v<std::remove_all_pointers<T>, T>
		// not caring about the const versions, they shouldn't be const in the first place
		// || !std::is_same<const std::remove_all_pointers<T>, T>::value
		// || !std::is_same<std::remove_all_pointers<T> const, T>::value
		, "ComVarT: template parameter not a pointer");

public:
	ComVarT(baseTypeT* _value) :
		m_var_ptr(_value)
	{
	}

	virtual ~ComVarT() = default;

	const baseTypeT& GetVal() const
	{
		return Get();
	}

	template<typename Type = baseTypeT>
	std::string AsString()
	{
		return std::to_string(*m_var_ptr);
	}

	template<>
	std::string AsString<bool>()
	{
		return *m_var_ptr ? "true" : "false";
	}

	std::string GetValStr() override
	{
		return AsString();
	}

	template<typename Type = baseTypeT>
	bool SetValFromStr(const std::string& str, int _Base = 10, std::string& potentialException = std::string())
	{
		bool success = true;
		try
		{
			SetValFromStrInternal<Type>(str, _Base);
		}
		catch (const std::exception& e)
		{
			success = false;
			potentialException.assign(e.what());
		}

		return success;
	}

	template<typename Type = baseTypeT>
	void SetVal(const Type _val)
	{
		baseTypeT& val = Get();
		val = _val;
	}

protected:
	// variable or pointer to variable
	baseTypeT* m_var_ptr;

	template<typename Type = baseTypeT>
	void SetValFromStrInternal(const std::string& str, int _Base = 10);

	template<>
	void SetValFromStrInternal<int>(const std::string& str, int _Base)
	{
		baseTypeT& val = Get();
		val = std::stoi(str, nullptr, _Base);
	}

	template<>
	void SetValFromStrInternal<unsigned int>(const std::string& str, int _Base)
	{
		baseTypeT& val = Get();
		val = std::stoul(str, nullptr, _Base);
	}

	template<>
	void SetValFromStrInternal<long long>(const std::string& str, int _Base)
	{
		baseTypeT& val = Get();
		val = std::stoll(str, nullptr, _Base);
	}

	// for floating point, _Base param does nothing
	template<>
	void SetValFromStrInternal<float>(const std::string& str, int _Base)
	{
		baseTypeT& val = Get();
		val = std::stof(str);
	}

	// for boolean, _Base param does nothing
	template<>
	void SetValFromStrInternal<bool>(const std::string& str, int _Base)
	{
		baseTypeT& val = Get();
		if (str != "true" 
			&& str != "false"
			&& str != "1"
			&& str != "0"
			)
			throw std::runtime_error("error: invalid boolean format (must be true/false)");
		else
			val = str == "true" ? true : false;
	}

	baseTypeT* GetPtr() const
	{
		return m_var_ptr;
	}

	baseTypeT& Get() const
	{
		return *GetPtr();
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
	// for custom types, the class should implement the equal operator overload
	// othewise default is used
	ComVar(T value) :
		ComVarT(&m_var)
	{
		m_var = value;
	}

	// TODO FIXME might want to set value defaults
	// based on the type using specializations
	ComVar() :
		ComVarT(&m_var)
	{
	}

	virtual ~ComVar() = default;
};