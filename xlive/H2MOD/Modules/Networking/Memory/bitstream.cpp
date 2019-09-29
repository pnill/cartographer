#include "stdafx.h"
#include "bitstream.h"
#include "H2MOD.h"

// sign flag
int8_t __SETS__(int x)
{
	if (sizeof(int) == 1)
		return int8_t(x) < 0;
	if (sizeof(int) == 2)
		return int16_t(x) < 0;
	if (sizeof(int) == 4)
		return int32_t(x) < 0;
	return int64_t(x) < 0;
}

// overflow flag of subtraction (x-y)
int8_t __OFSUB__(int x, int y)
{
	int y2 = y;
	int8_t sx = __SETS__(x);
	return (sx ^ __SETS__(y2)) & (sx ^ __SETS__(x - y2));
}

data_decode_string bitstream::p_data_decode_string() {
	return h2mod->GetAddress<data_decode_string>(0xD1FA7, 0xCE561);
}

data_decode_address bitstream::p_data_decode_address() {
	return reinterpret_cast<data_decode_address>(h2mod->GetAddress(0xD1FFD, 0xCE5B7));
}

data_decode_bits bitstream::p_data_decode_bits() {
	return h2mod->GetAddress<data_decode_bits>(0xD1F95, 0xCE54F);
}

data_decode_integer bitstream::p_data_decode_integer() {
	return h2mod->GetAddress<data_decode_integer>(0xD1EE5, 0xCE49F);
}

data_decode_bool bitstream::p_data_decode_bool() {
	return h2mod->GetAddress< data_decode_bool>(0xD1F47, 0xCE501);
}

data_encode_integer bitstream::p_data_encode_integer() {
	return h2mod->GetAddress<data_encode_integer>(0xD17C6, 0xCDD80);
}

data_encode_string bitstream::p_data_encode_string() {
	return h2mod->GetAddress<data_encode_string>(0xD18DF, 0xCDE99);
}

data_encode_bits bitstream::p_data_encode_bits() {
	return h2mod->GetAddress<data_encode_bits>(0xD18CD, 0xCDE87);
}

data_encode_boolean bitstream::p_data_encode_bool() {
	return h2mod->GetAddress<data_encode_boolean>(0xD1886, 0xCDE40);
}

valid_packet_type bitstream::p_packet_is_valid() {
	return h2mod->GetAddress<valid_packet_type>(0xD114C, 0xCD706);
}


