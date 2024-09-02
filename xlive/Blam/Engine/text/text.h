#pragma once

enum e_text_justification : uint16
{
    text_justification_left = 0,
    text_justification_center = 1,
    text_justification_right = 2,
};

enum e_font_index : int8
{
    font_index_defualt = 0,
    font_index_number_font = 1,
    font_index_invalid = -1
};

enum e_text_font : uint16
{
    text_font_terminal_font = 0,
    text_font_body_text_font = 1,
    text_font_title_font = 2,
    text_font_super_large_font = 3,
    text_font_large_body_text_font = 4,
    text_font_split_screen_hud_message_font = 5,
    text_font_full_screen_hud_message_font = 6,
    text_font_english_body_text_font = 7,
    text_font_hud_number_font = 8,
    text_font_subtitle_font = 9,
    text_font_main_menu_font = 10,
    text_font_text_chat_font = 11
};