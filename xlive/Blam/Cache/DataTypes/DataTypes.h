#pragma once
#include "DatumIndex.h"
#include "Reflexive.h"
#include "Pad.h"
#include "StringID.h"
#include "Strings.h"
#include "TagRef.h"
#include "DataRef.h"

struct blam_tag
{
	union {
		char c_data[4];
		uint32_t i_data;
	};

	blam_tag()
	{
	}

	constexpr blam_tag(uint32_t data) :
		i_data(data)
	{
	}

	inline std::string as_string() const
	{
		if (is_none())
			return "NONE";
		if (is_null())
			return "";
		std::string out;
		out += c_data[3];
		out += c_data[2];
		out += c_data[1];
		out += c_data[0];
		return out;
	}

	constexpr int as_int() const
	{
		return i_data;
	}

	constexpr bool is_null() const
	{
		return as_int() == NULL;
	}

	constexpr bool is_none() const
	{
		return as_int() == 0xFFFFFFFFF;
	}

	constexpr bool is_set() const
	{
		return !is_null() && !is_none();
	}

	constexpr bool is_printable() const
	{
		return isprint(c_data[0]) && isprint(c_data[1]) && isprint(c_data[2]) && isprint(c_data[3]);
	}

	constexpr bool operator==(const blam_tag &other) const
	{
		return this->as_int() == other.as_int();
	}

	constexpr bool operator!=(const blam_tag &other) const
	{
		return this->as_int() != other.as_int();
	}

	constexpr static blam_tag none()
	{
		return blam_tag(0xFFFFFFFF);
	}
};

