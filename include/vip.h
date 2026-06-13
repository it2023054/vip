#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "editor.h"
#include "line.h"
#include "terminal.h"

void cleanup(void);
void run_vip(char* filename);

Line* get_line(size_t row);
char* get_command_line(void);
size_t get_command_line_cursor_col(void);
char* get_key_cache_string(void);
char* get_cursor_position_string(void);
size_t get_line_count(void);
Position get_cursor_position(void);
Position get_text_position(void);
bool has_number(void);
bool has_relative_number(void);
bool is_in_start(void);
Mode get_mode(void);
