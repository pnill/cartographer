#pragma once

#include "stdafx.h"

/// Command Variable

// useful macros
#define DECL_ComVarCommandPtr(_var_name, _var_type, _var_ptr, _command_name, _description, _parameter_count, _callback) \
ComVarTPtr<_var_type> _var_name(_var_ptr); \
ConsoleVarCommand _var_name## _cmd(_command_name, _description, _parameter_count, _callback, (ComVar*)&_var_name);

class ComVar
{
public:
	virtual ~ComVar() = default;

	virtual std::string GetValStr()
	{
		return "<unimplemented>";
	}

	virtual void UpdateVarPtr(void* updateVarPtr)
	{
		return;
	}
private:
};

template<typename T, bool base_type_is_ptr = std::is_pointer<T>::value>
class ComVarT : public ComVar
{
	using baseTypeT = std::remove_pointer_t<T>;

#define SETVAR_CHECK_PTR_TYPE_VALID() \
if CONSTEXPR (base_type_is_ptr) \
{ \
	if (GetPtr() == nullptr) \
		return; \
}

public:
	ComVarT()
	{
		if CONSTEXPR(base_type_is_ptr)
			m_var = nullptr;
	}

	ComVarT(T _value)
	{
		m_var = _value;
	}

	virtual ~ComVarT() = default;

	const baseTypeT& GetVal() const
	{
		return Get();
	}

	template<typename Type = baseTypeT>
	std::string AsString()
	{
		if CONSTEXPR(base_type_is_ptr)
			return std::to_string(*m_var);
		else
			return std::to_string(m_var);
	}

	template<>
	std::string AsString<bool>()
	{
		if CONSTEXPR(base_type_is_ptr)
			return *m_var ? "true" : "false";
		else
			return m_var ? "true" : "false";
	}

	std::string GetValStr() override
	{
		return AsString();
	}

	void UpdateVarPtr(void* updateVarPtr) override
	{
		if CONSTEXPR(base_type_is_ptr)
			m_var = (T)updateVarPtr;
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
		SETVAR_CHECK_PTR_TYPE_VALID();
		baseTypeT& val = Get();
		val = _val;
	}

protected:
	// variable or pointer to variable
	T m_var;

	template<typename Type = baseTypeT>
	void SetValFromStrInternal(const std::string& str, int _Base = 10);

	template<>
	void SetValFromStrInternal<int>(const std::string& str, int _Base)
	{
		SETVAR_CHECK_PTR_TYPE_VALID();
		baseTypeT& val = Get();
		val = std::stoi(str, nullptr, _Base);
	}

	template<>
	void SetValFromStrInternal<long long>(const std::string& str, int _Base)
	{
		SETVAR_CHECK_PTR_TYPE_VALID();
		baseTypeT& val = Get();
		val = std::stoll(str, nullptr, _Base);
	}

	// for floating point, _Base param does nothing
	template<>
	void SetValFromStrInternal<float>(const std::string& str, int _Base)
	{
		SETVAR_CHECK_PTR_TYPE_VALID();
		baseTypeT& val = Get();
		val = std::stof(str);
	}

	// for boolean, _Base param does nothing
	template<>
	void SetValFromStrInternal<bool>(const std::string& str, int _Base)
	{
		SETVAR_CHECK_PTR_TYPE_VALID();
		baseTypeT& val = Get();
		if (str != "true" && str != "false")
			throw std::runtime_error("error: invalid boolean format (must be true/false)");
		else
			val = str == "true" ? true : false;
	}

	T GetPtr()
	{
		if CONSTEXPR(base_type_is_ptr)
			return m_var;

		return nullptr;
	}

	baseTypeT& Get()
	{
		if CONSTEXPR(base_type_is_ptr)
		{
			return *m_var;
		}
		else
		{
			return m_var;
		}
	}

	const baseTypeT& Get() const
	{
		if CONSTEXPR(base_type_is_ptr)
		{
			return *m_var;
		}
		else
		{
			return m_var;
		}
	}
};

// same as ComVarT, but holds a pointer to the data
template<typename T>
class ComVarTPtr : public ComVarT<T, true>
{
	static_assert(std::is_pointer<T>::value, "ComVarTPtr: template parameter not a pointer");

public:
	ComVarTPtr(T _var_ptr) :
		ComVarT(_var_ptr)
	{
	}

	ComVarTPtr(std::remove_pointer_t<T> _default_value, T _var_ptr) :
		ComVarT(_var_ptr)
	{
		*m_val = default_value;
	}

private:

#undef SETVAR_CHECK_PTR_TYPE_VALID
};
