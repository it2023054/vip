#pragma once

#include "terminal.h"

#define KEY_CACHE_STRING_SIZE 10
#define CURSOR_POSITION_STRING_SIZE 10

void enter_alt_screen(void);
void exit_alt_screen(void);

void move_cursor(Position pos);
void fix_text_position(Position cursor, Position* text, const size_t line_count);
void clean_screen(void);
void clean_line(void);
void draw_window(void);
void refresh_window(void);
