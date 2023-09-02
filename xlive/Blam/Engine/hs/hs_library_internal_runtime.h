#pragma once
#include "hs.h"

enum e_syntax_node_flags : WORD
{
	syntax_node_flag_primitive = FLAG(0),
	syntax_node_flag_user_function = FLAG(1),
	syntax_node_flag_global_reference = FLAG(2),
	syntax_node_flag_bit_4 = FLAG(3),
	syntax_node_flag_bit_5 = FLAG(4),
	syntax_node_flag_bit_6 = FLAG(5),
	syntax_node_flag_bit_7 = FLAG(6),
	syntax_node_flag_bit_8 = FLAG(7),
	syntax_node_flag_bit_9 = FLAG(8),
	syntax_node_flag_bit_10 = FLAG(9),
	syntax_node_flag_bit_11 = FLAG(10),
	syntax_node_flag_bit_12 = FLAG(11),
	syntax_node_flag_bit_13 = FLAG(12),
	syntax_node_flag_bit_14 = FLAG(13),
	syntax_node_flag_bit_15 = FLAG(14),
	syntax_node_flag_bit_16 = FLAG(15)
};
// max count: k_maximum_hs_syntax_nodes_per_scenario 36864
struct hs_syntax_node
{
    WORD salt;
    short opcode;
    e_hs_type type;
	
	e_syntax_node_flags expression_type;
	datum next_expression;
	int string_address;
	char value_00_lsb;
	char value_01;
	char value_02;
	char value_03_msb;
};
TAG_BLOCK_SIZE_ASSERT(hs_syntax_node, 20);